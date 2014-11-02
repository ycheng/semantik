// Thomas Nagy 2007-2014 GPLV3

#include <QtDebug>
#include <QString>
#include <QCoreApplication>
#include <QtDebug>
#include <QTextEdit>
#include <QKeyEvent>
#include <QScrollBar>
#include <QCompleter>
#include <QTreeView>
#include <QHeaderView>
#include <QAction>
#include <QStandardItemModel>
#include "con.h"

 #include "sembind.h"
#include "sem_mediator.h"
#include "data_item.h"


node::node() {

}

node::~node()
{
	while (!m_oChildren.isEmpty()) {
		m_oChildren.takeFirst();
	}
}

node& node::make_node(const QString& i_sTag, const QXmlAttributes& i_oAttrs) {
	node l_oNode;
	m_oChildren.push_back(l_oNode);
	node &ret = m_oChildren.last();
	ret.read_data(i_sTag, i_oAttrs);
	return ret;
}

void node::read_data(const QString&, const QXmlAttributes&)
{

}

void node::dump_xml(QStringList & other)
{
	foreach (node n, m_oChildren) {
		n.dump_xml(other);
	}
}

data_item::data_item(sem_mediator *i_oControl, int i_iId)
{
	m_iDataType = VIEW_TEXT;

	m_iId = i_iId;
	m_iColor = 0;
	m_oMediator = i_oControl;
	m_sSummary = QObject::trUtf8("Empty");
	m_sText = "";
	m_sComment = "";
	m_sDiag = "";

	m_sPicLocation = "";
	m_sPicCaption = "";
	m_sPicComment = "";

	m_iDown = NO_ITEM;

	m_iNumRows = 3;
	m_iNumCols = 3;
	m_iPicId = NO_ITEM;

	m_iObjectWidthHint = 0;
	m_iObjectHeightHint = 0;

	m_iYY = m_iXX = -200000.0;
	m_bSelected = false;
}

QPixmap data_item::getPix()
{
	return m_oMediator->getPix(m_iPicId);
}

QPixmap data_item::getThumb()
{
	return m_oMediator->getThumb(m_iPicId);
}


color_scheme& data_item::get_color_scheme()
{
	if (m_oMediator->parent_of(m_iId) <= 0)
	{
		return m_oMediator->m_oColorSchemes[0];
	}
	if (m_iColor < 0)
	{
		return m_oCustom;
	}
	if (m_iColor == m_oMediator->m_oColorSchemes.size())
	{
		return m_oCustom;
	}
	if (m_iColor > m_oMediator->m_oColorSchemes.size())
	{
		qDebug()<<"invalid index color scheme "<<m_iColor;
		return m_oCustom;
	}
	if (m_iColor == 0)
	{
		return m_oMediator->m_oColorSchemes[1];
	}
	return m_oMediator->m_oColorSchemes[m_iColor];
}

color_scheme& data_item::get_color_scheme_raw()
{
	if (m_iColor < 0)
	{
		return m_oCustom;
	}
	if (m_iColor == m_oMediator->m_oColorSchemes.size())
	{
		return m_oCustom;
	}
	if (m_iColor > m_oMediator->m_oColorSchemes.size())
	{
		qDebug()<<"invalid index color scheme "<<m_iColor;
		return m_oCustom;
	}
	return m_oMediator->m_oColorSchemes[m_iColor];
}


diagram_item::diagram_item()
{
	pen_style = Qt::SolidLine;
	border_width = 1;
}

data_box::data_box(int id) : diagram_item(), node()
{
	m_iId = id;
	m_bIsEnd = false;
	m_bIsVertical = false;
	m_iType = data_box::ACTIVITY;
	m_iWW = 100;
	m_iHH = 40;
	color = QColor("#a7e89b");
}

data_box::~data_box()
{
	while (!m_oMethods.isEmpty()) {
		m_oMethods.takeFirst();
	}
	while (!m_oAttributes.isEmpty()) {
		m_oAttributes.takeFirst();
	}
}

void data_box::dump_xml(QStringList & i_oS)
{
	i_oS<<notr("<itembox id=\"%1\" text=\"%2\" x=\"%3\" y=\"%4\" w=\"%5\" h=\"%6\" color=\"%7\" t=\"%8\" %9>\n").arg(
		QString::number(m_iId),
		bind_node::protectXML(m_sText),
		QString::number(m_iXX),
		QString::number(m_iYY),
		QString::number(m_iWW),
		QString::number(m_iHH),
		color.name(),
		QString::number((int) m_iType),
		QString(" v=\"%1\" e=\"%2\"").arg( // ugly
			QString::number((int) m_bIsVertical),
			QString::number((int) m_bIsEnd)
		)
	);

	foreach (int l_i, m_oRowSizes) {
		i_oS<<notr("      <box_row_size val=\"%1\"/>\n").arg(QString::number(l_i));
	}
	foreach (int l_i, m_oColSizes) {
		i_oS<<notr("      <box_col_size val=\"%1\"/>\n").arg(QString::number(l_i));
	}

	if (!m_sStereotype.isNull()) {
		i_oS<<notr("      <box_stereotype text=\"%1\"/>\n").arg(m_sStereotype);
	}
	foreach (data_box_method l_o, m_oMethods) {
		l_o.dump_xml(i_oS);
	}
	foreach (data_box_attribute l_o, m_oAttributes) {
		l_o.dump_xml(i_oS);
	}
	//node::dump_xml(i_oS);
	i_oS<<notr("</itembox>\n");
}

void data_box::read_data(const QString& i_sTag, const QXmlAttributes& i_oAttrs)
{
	m_sText = i_oAttrs.value(notr("text"));
	m_iXX = i_oAttrs.value(notr("x")).toFloat();
	m_iYY = i_oAttrs.value(notr("y")).toFloat();
	m_iWW = i_oAttrs.value(notr("w")).toFloat();
	m_iHH = i_oAttrs.value(notr("h")).toFloat();
	m_iType = (data_box::IType) i_oAttrs.value(notr("t")).toInt();
	m_bIsVertical = i_oAttrs.value(notr("v")).toInt();
	m_bIsEnd = i_oAttrs.value(notr("e")).toInt();
	color = QColor(i_oAttrs.value(notr("color")));

	// TODO remove in the future...
	if (m_iType == data_box::ACTIVITY_START)
	{
		if (m_iWW > 20 + m_iHH) m_iWW = m_iHH = 20;
	}
}

node& data_box::make_node(const QString& i_sName, const QXmlAttributes& i_oAttrs)
{
	if (i_sName == notr("box_row_size"))
	{
		int l_iVal = i_oAttrs.value(notr("val")).toInt();
		m_oRowSizes.push_back(l_iVal);
	}
	else if (i_sName == notr("box_col_size"))
	{
		int l_iVal = i_oAttrs.value(notr("val")).toInt();
		m_oColSizes.push_back(l_iVal);
	}
	else if (i_sName == notr("box_class_method"))
	{
		data_box_method l_o; // = new data_box_method();
		l_o.read_data(i_sName, i_oAttrs);
		m_oMethods.push_back(l_o);
		return m_oMethods.last();
	}
	else if (i_sName == notr("box_class_attribute"))
	{
		data_box_attribute l_o; // = new data_box_attribute();
		l_o.read_data(i_sName, i_oAttrs);
		m_oAttributes.push_back(l_o);
		return m_oAttributes.last();
	}
	else if (i_sName == notr("box_stereotype"))
	{
		m_sStereotype = i_oAttrs.value(notr("text"));
	}
	return *this;
	// return node::make_node(i_sName, i_oAttrs);
}

void diagram_item::setBorderWidth(int bw)
{
	border_width = bw;
}

void diagram_item::setPenStyle(Qt::PenStyle st)
{
	pen_style = st;
}

void diagram_item::setColor(QColor co)
{
	color = co;
}

data_link::data_link() : diagram_item()
{
	color = QColor(Qt::black);
	m_iParentPos = NORTH;
	m_iChildPos = NORTH;
	m_iLeftArrow = NONE;
	m_iRightArrow = ASSOCIATION;
	m_iLineType = ZIGZAG;
}

void data_link::copy_from(const data_link& i_oLink)
{
	m_sLink       = i_oLink.m_sLink;
	m_sCaption    = i_oLink.m_sCaption;
	m_iParent     = i_oLink.m_iParent;
	m_iChild      = i_oLink.m_iChild;
	m_iParentPos  = i_oLink.m_iParentPos;
	m_iChildPos   = i_oLink.m_iChildPos;
	m_iLineType   = i_oLink.m_iLineType;
	m_oStartPoint = i_oLink.m_oStartPoint;
	m_oEndPoint   = i_oLink.m_oEndPoint;
	m_oOffsets    = i_oLink.m_oOffsets;

	m_iLeftArrow  = i_oLink.m_iLeftArrow;
	m_iRightArrow = i_oLink.m_iRightArrow;

	border_width  = i_oLink.border_width;
	pen_style     = i_oLink.pen_style;
	color         = i_oLink.color;
}

bool data_link::equals(const data_link& i_oLink)
{
	if (m_iParent != NO_ITEM)
	{
		if (m_iParent != i_oLink.m_iParent || m_iParentPos != i_oLink.m_iParentPos)
			return false;
	}
	else
	{
		if (m_oStartPoint != i_oLink.m_oStartPoint)
			return false;
	}

	if (m_iChild != NO_ITEM)
	{
		if (m_iChild != i_oLink.m_iChild || m_iChildPos != i_oLink.m_iChildPos)
			return false;
	}
	else
	{
		if (m_oEndPoint != i_oLink.m_oEndPoint)
			return false;
	}

	if (m_oOffsets.size() != i_oLink.m_oOffsets.size())
		return false;

	for (int i = 0; i < m_oOffsets.size(); ++i)
	{
		if (m_oOffsets.at(i) != i_oLink.m_oOffsets.at(i))
			return false;
	}

	return true;
}

void data_box_method::read_data(const QString&i_sName, const QXmlAttributes& i_oAttrs)
{
	Q_ASSERT(i_sName == QString("box_class_method"));
	m_sText = i_oAttrs.value(notr("text"));
	m_oVisibility = (visibility::VisibilityType) i_oAttrs.value(notr("visibility")).toInt();
	m_bStatic = i_oAttrs.value(notr("static")).toInt();
        m_bAbstract = i_oAttrs.value(notr("abstract")).toInt();
}

void data_box_method::dump_xml(QStringList &i_oS)
{
	i_oS<<notr("       <box_class_method text=\"%1\" visibility=\"%2\" static=\"%3\" abstract=\"%4\">\n").arg(
		m_sText,
		QString::number((int) m_oVisibility),
		QString::number((int) m_bStatic),
		QString::number((int) m_bAbstract)
	);
	//node::dump_xml(i_oS);
	i_oS<<notr("      </box_class_method>\n");
}

void data_box_attribute::read_data(const QString& i_sName, const QXmlAttributes& i_oAttrs)
{
	Q_ASSERT(i_sName == QString("box_class_attribute"));
	m_sText = i_oAttrs.value(notr("text"));
	m_oVisibility = (visibility::VisibilityType) i_oAttrs.value(notr("visibility")).toInt();
	m_bStatic = i_oAttrs.value(notr("static")).toInt();
}

void data_box_attribute::dump_xml(QStringList &i_oS)
{
	i_oS<<notr("      <box_class_attribute text=\"%1\" visibility=\"%2\" static=\"%3\">\n").arg(
		m_sText,
		QString::number((int) m_oVisibility),
		QString::number((int) m_bStatic)
	);
	//node::dump_xml(i_oS);
	i_oS<<notr("      </box_class_attribute>\n");
}


/*
#include "main.moc"
*/
