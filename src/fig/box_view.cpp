// Thomas Nagy 2007-2011 GPLV3

#include <QMouseEvent>
#include <QAction>
#include <QGraphicsScene>
#include <QApplication>
#include<QList>
#include <QScrollBar>
#include <QMenu>
#include<QTextDocument>
#include <QColorDialog>
#include <QtDebug>
#include <QtDebug>
#include <QTextEdit>
#include <QKeyEvent>
#include <QScrollBar>
#include <QCompleter>
#include <QTreeView>
#include <QHeaderView>
#include <QAction>
#include <QStandardItemModel>

#include "aux.h"
#include "con.h"

#include <math.h>
#include "sem_model.h"
#include "box_item.h"
#include "box_link.h"
#include "data_item.h"
#include "box_view.h"
#include "sembind.h"
 #include "mem_box.h"

class box_reader : public QXmlDefaultHandler
{
    public:
	box_reader(box_view*);

	QString m_sBuf;
	int m_iVersion;
	box_view *m_oControl;
	box_link *m_oCurrent;
	int m_iId;

	bool startElement(const QString&, const QString&, const QString&, const QXmlAttributes&);
	bool endElement(const QString&, const QString&, const QString&);
	bool characters(const QString &i_sStr);
};

box_reader::box_reader(box_view *i_oControl)
{
	m_oControl = i_oControl;
}

bool box_reader::startElement(const QString&, const QString&, const QString& i_sName, const QXmlAttributes& i_oAttrs)
{
	if (i_sName == QObject::trUtf8("box_item"))
	{
		int id = i_oAttrs.value(QObject::trUtf8("id")).toInt();

		data_box *box = new data_box(id);
		m_oControl->m_oControl->m_oItems[m_iId]->m_oBoxes[id] = box;
		box->m_iXX = i_oAttrs.value(QObject::trUtf8("c1")).toFloat();
		box->m_iYY = i_oAttrs.value(QObject::trUtf8("c2")).toFloat();
		box->m_sText = i_oAttrs.value(QObject::trUtf8("text"));
		box->fill_color = i_oAttrs.value(QObject::trUtf8("col"));
		//l_o->setRect(QRectF(0., 0., i_oAttrs.value(QObject::trUtf8("c3")).toDouble(), i_oAttrs.value(QObject::trUtf8("c4")).toDouble()));
	}
	else if (i_sName == QObject::trUtf8("box_link"))
	{
		/* box_link *l_o = new box_link(m_oControl);
		l_o->m_iParent = i_oAttrs.value(QObject::trUtf8("p1")).toInt();
		l_o->m_iChild = i_oAttrs.value(QObject::trUtf8("p2")).toInt();

		int l_i = i_oAttrs.value(QObject::trUtf8("c1")).toInt();
		foreach (box_item* l_oItem, m_oControl->m_oItems)
		{
			if (l_oItem->m_iId == l_i)
			{
				l_o->m_oParent = l_oItem; break;
			}
		}

		l_i = i_oAttrs.value(QObject::trUtf8("c2")).toInt();
		foreach (box_item* l_oItem, m_oControl->m_oItems)
		{
			if (l_oItem->m_iId == l_i)
			{
				l_o->m_oChild = l_oItem; break;
			}
		}

		QPen l_oPen;
		l_oPen.setWidth(i_oAttrs.value(QObject::trUtf8("wl")).toInt());
		l_oPen.setStyle((Qt::PenStyle) i_oAttrs.value(QObject::trUtf8("sl")).toInt());
		if (l_oPen.style() == Qt::NoPen) l_oPen.setStyle(Qt::SolidLine);
		l_o->setPen(l_oPen);

		if (!l_o->m_oChild or !l_o->m_oParent)
		{
			delete l_o;
			return false;
		}

		m_oControl->m_oLinks.push_back(l_o);
		m_oCurrent = l_o; */
	}
	else if (i_sName == QObject::trUtf8("box_link_offset"))
	{
		if (m_oCurrent)
		{
			m_oCurrent->m_oOffsets.append(
				QPoint(i_oAttrs.value(QObject::trUtf8("x")).toInt(), i_oAttrs.value(QObject::trUtf8("y")).toInt()));
		}
	}

	return true;
}

bool box_reader::endElement(const QString&, const QString&, const QString& i_sName)
{
	if (i_sName == QObject::trUtf8("box_link"))
	{
		if (m_oCurrent) m_oCurrent->update_pos();
		m_oCurrent = NULL;
	}
	return true;
}

bool box_reader::characters(const QString &i_s)
{
	m_sBuf += i_s;
	return true;
}

int box_view::next_id()
{
	return ++m_iIdCounter;
}

box_view::box_view(QWidget *i_oWidget, sem_model *i_oControl) : QGraphicsView(i_oWidget)
{
	m_oControl = i_oControl;
	m_iIdCounter = 0; // hidden
	m_iId = NO_ITEM;

	QGraphicsScene *l_oScene = new QGraphicsScene(this);
	l_oScene->setSceneRect(-400, -400, 400, 400);
	setScene(l_oScene);

	//setCacheMode(CacheBackground);
	setRenderHint(QPainter::Antialiasing);

	m_oEditAction = new QAction(QObject::trUtf8("Toggle edit"), this);
	m_oEditAction->setShortcut(QObject::trUtf8("Return"));
	connect(m_oEditAction, SIGNAL(triggered()), this, SLOT(slot_toggle_edit()));
	addAction(m_oEditAction);

	m_oCancelEditAction = new QAction(trUtf8("Cancel edit"), this);
	m_oCancelEditAction->setShortcut(notr("Escape"));
	addAction(m_oCancelEditAction);
	connect(m_oCancelEditAction, SIGNAL(triggered()), this, SLOT(slot_cancel_edit()));
	m_oCancelEditAction->setEnabled(false);

	m_oAddItemAction = new QAction(QObject::trUtf8("Add Box"), this);
	m_oAddItemAction->setShortcut(QObject::trUtf8("Ctrl+Return"));
	connect(m_oAddItemAction, SIGNAL(triggered()), this, SLOT(slot_add_item()));
	addAction(m_oAddItemAction);

	m_oDeleteAction = new QAction(QObject::trUtf8("Delete selection"), this);
	m_oDeleteAction->setShortcut(QObject::trUtf8("Delete"));
	connect(m_oDeleteAction, SIGNAL(triggered()), this, SLOT(slot_delete()));
	addAction(m_oDeleteAction);

	m_oColorAction = new QAction(QObject::trUtf8("Color..."), this);
	connect(m_oColorAction, SIGNAL(triggered()), this, SLOT(slot_color()));
	addAction(m_oColorAction);


	//m_oMoveUpAction = new QAction(QObject::trUtf8("Move up"), this);
	//m_oMoveUpAction->setShortcut(QObject::trUtf8("PgUp"));
	//connect(m_oMoveUpAction, SIGNAL(triggered()), this, SLOT(slot_move_up()));
	//addAction(m_oMoveUpAction);

	//m_oMoveDownAction = new QAction(QObject::trUtf8("Move down"), this);
	//m_oMoveDownAction->setShortcut(QObject::trUtf8("PgDown"));
	//connect(m_oMoveDownAction, SIGNAL(triggered()), this, SLOT(slot_move_down()));
	//addAction(m_oMoveDownAction);


	m_oMenu = new QMenu(this);
	m_oMenu->addAction(m_oAddItemAction);
	m_oMenu->addAction(m_oEditAction);
	m_oMenu->addAction(m_oDeleteAction);
	m_oMenu->addAction(m_oColorAction);
	//m_oMenu->addAction(m_oMoveUpAction);
	//m_oMenu->addAction(m_oMoveDownAction);

	QAction *l_o;

	m_oWidthMenu = m_oMenu->addMenu(QObject::trUtf8("Connection width"));
	m_oWidthGroup = new QActionGroup(this);
	l_o = m_oWidthMenu->addAction(QObject::trUtf8("1px")); connect(l_o, SIGNAL(triggered()), this, SLOT(slot_penwidth())); addAction(l_o); l_o->setData(QVariant(1)); m_oWidthGroup->addAction(l_o);
	l_o = m_oWidthMenu->addAction(QObject::trUtf8("2px")); connect(l_o, SIGNAL(triggered()), this, SLOT(slot_penwidth())); addAction(l_o); l_o->setData(QVariant(2)); m_oWidthGroup->addAction(l_o);
	l_o = m_oWidthMenu->addAction(QObject::trUtf8("3px")); connect(l_o, SIGNAL(triggered()), this, SLOT(slot_penwidth())); addAction(l_o); l_o->setData(QVariant(3)); m_oWidthGroup->addAction(l_o);

	m_oStyleMenu = m_oMenu->addMenu(QObject::trUtf8("Connection style"));
	m_oStyleGroup = new QActionGroup(this);
	l_o = m_oStyleMenu->addAction(QObject::trUtf8("solid line")); connect(l_o, SIGNAL(triggered()), this, SLOT(slot_penstyle())); addAction(l_o); l_o->setData(QVariant(Qt::SolidLine)); m_oStyleGroup->addAction(l_o);
	l_o = m_oStyleMenu->addAction(QObject::trUtf8("dot line")); connect(l_o, SIGNAL(triggered()), this, SLOT(slot_penstyle())); addAction(l_o); l_o->setData(QVariant(Qt::DotLine)); m_oStyleGroup->addAction(l_o);
	l_o = m_oStyleMenu->addAction(QObject::trUtf8("dash line")); connect(l_o, SIGNAL(triggered()), this, SLOT(slot_penstyle())); addAction(l_o); l_o->setData(QVariant(Qt::DashLine)); m_oStyleGroup->addAction(l_o);


	m_oAddItemAction->setEnabled(false);
	m_oDeleteAction->setEnabled(false);

	qDebug()<<"disable edit action";
	m_oEditAction->setEnabled(false);
	//m_oMoveUpAction->setEnabled(false);
	//m_oMoveDownAction->setEnabled(false);
	m_oMenu->addSeparator();
	m_oColorAction->setEnabled(false);

	m_oLastMovePoint = QPointF(-100, -100);

	m_oCurrent = NULL;
	m_bPressed = false;
	m_bScroll = false;
}

box_view::~box_view()
{

}

void box_view::notify_add_item(int id)
{
	m_iId = id;
	setEnabled(true);
}

void box_view::notify_edit_box(int id, int bid)
{

}

void box_view::notify_select(const QList<int>& unsel, const QList<int>& sel) {

	/*int l_iOldId = m_iId;
	if (l_iOldId)
	{
		data_item *l_oData = m_oControl->m_oItems.value(l_iOldId);
		if (l_oData != NULL and l_oData->m_iDataType == VIEW_DIAG) {
			l_oData->m_sDiag = to_string();
		}
	}*/
	clear_diagram();

	if (sel.size() != 1)
	{
		m_iId = NO_ITEM;
		setEnabled(false);
		qDebug()<<"disable edit action";
		m_oEditAction->setEnabled(false);
		m_oCancelEditAction->setEnabled(false);
	}
	else
	{
		m_iId = sel.at(0);
		data_item *l_oData = m_oControl->m_oItems.value(m_iId);
		Q_ASSERT(l_oData);
		if (l_oData and l_oData->m_iDataType == VIEW_DIAG)
		{
			if (!l_oData->m_sDiag.isEmpty())
			{
				from_string(l_oData->m_sDiag);
				l_oData->m_sDiag = "";
			}
			sync_view();
			check_canvas_size();
		}
		setEnabled(true);
		m_oEditAction->setEnabled(true);
	}
}

void box_view::sync_view()
{
	if (!m_iId) return;
	
	data_item *item = m_oControl->m_oItems.value(m_iId);
	Q_ASSERT(item);
	if (item->m_iDataType != VIEW_DIAG)
	{
		return;
	}
	if (!item->m_sDiag.isEmpty())
	{
		from_string(item->m_sDiag);
		item->m_sDiag = "";
	}

	foreach (data_box *box, item->m_oBoxes.values()) {
		box_item *l_o = new box_item(this, box->m_iId);
		m_oItems[box->m_iId] = l_o;
		l_o->setPlainText(box->m_sText);
		l_o->setPos(QPointF(box->m_iXX, box->m_iYY));
		l_o->update_data();
	}

	foreach (data_link *link, item->m_oLinks) {
		box_link *l_o = new box_link(this);
		l_o->set_link(link);
		l_o->update_pos();
		m_oLinks.append(l_o);
	}
}

void box_view::notify_export_item(int id)
{
	int l_iOldId = m_iId;
	clear_diagram();

	m_iId = id;
	sync_view();

	QRectF l_oRect = scene()->itemsBoundingRect();
	l_oRect = QRectF(l_oRect.topLeft() - QPointF(25, 25), l_oRect.bottomRight() + QPointF(25, 25));

	QRectF l_oR(0, 0, l_oRect.width(), l_oRect.height());


	// fill with white
	QImage l_oImage((int) l_oR.width(), (int) l_oR.height(), QImage::Format_RGB32);
	l_oImage.fill(qRgb(255,255,255));

	QPainter l_oP;
	l_oP.begin(&l_oImage);
	l_oP.setRenderHints(QPainter::Antialiasing);
	scene()->render(&l_oP, l_oR, l_oRect);
	l_oP.end();

	l_oImage.save(QString(m_oControl->m_sTempDir + QString("/") +
		QString("pic-%1.png")).arg(QString::number(m_iId)));
	clear_diagram();
	m_iId = l_iOldId;
	if (m_iId != NO_ITEM)
	{
		sync_view();
	}
}

void box_view::deselect_all()
{
	if (m_oSelected.size() == 1)
	{
		QFocusEvent l_oEv = QFocusEvent(QEvent::FocusOut);
	}

	while (m_oSelected.size() > 0)
	{
		QGraphicsItem *l_oItem = m_oSelected.takeFirst();
		l_oItem->update();
	}
	enable_menu_actions();
}

void box_view::add_select(QGraphicsItem* i_oItem)
{
	if (m_oSelected.contains(i_oItem))
	{
		qDebug()<<"item already selected";
		return;
	}

	m_oSelected.push_back(i_oItem);
	i_oItem->update();
	enable_menu_actions();

	if (m_oSelected.size() == 2)
	{
		// we do this if the item selected is a box item to show/hide the resize handle
		m_oSelected[0]->update();
	}
}

void box_view::rm_select(QGraphicsItem* i_oItem)
{
	m_oSelected.removeAll(i_oItem);
	i_oItem->update();
	enable_menu_actions();

	if (m_oSelected.size() == 1)
	{
		// we do this if the item selected is a box item to show/hide the resize handle
		m_oSelected[0]->update();
	}
}


void box_view::slot_delete()
{
	QList<int> l_oLst;
	foreach (box_item *l_oItem, selection()) {
		l_oLst.push_back(l_oItem->m_iId);
	}

	if (l_oLst.isEmpty()) {
		return;
	}

	//mem_sel *sel = new mem_sel(m_oControl);
	//sel->sel;
	//sel->unsel = l_oLst;
	//sel->apply();

	mem_del_box *del = new mem_del_box(m_oControl);
	del->init(m_iId, l_oLst);
	del->apply();
}

QList<box_item*> box_view::selection() {
	// FIXME use scene()->selectedItems()
	QList<box_item*> ret;
	foreach (QGraphicsItem *tmp, items()) {
		if (tmp->type() == BOX_ITEM_T && tmp->isSelected()) {
			ret.append((box_item*) tmp);
		}
	}
	return ret;
}

void box_view::enable_menu_actions()
{
	QList<QGraphicsItem*> selection = scene()->selectedItems();
	int selected = selection.size();

	m_oAddItemAction->setEnabled(selected <= 1);
	m_oDeleteAction->setEnabled(selected >= 1);
	m_oColorAction->setEnabled(selected >= 1);

	m_oEditAction->setEnabled(selected == 1 and selection.at(0)->type() == BOX_ITEM_T);

	m_oWidthMenu->setEnabled(selected >= 1 and selection.at(0)->type() == BOX_LINK_T);
	foreach(QAction* l_o, m_oWidthGroup->actions())
	{
		l_o->setEnabled(selected >= 1);
	}

	m_oStyleMenu->setEnabled(selected >= 1 and selection.at(0)->type() == BOX_LINK_T);
	foreach(QAction* l_o, m_oStyleGroup->actions())
	{
		l_o->setEnabled(selected >= 1);
	}
}

void box_view::enable_actions()
{
	foreach (QAction* l_o, actions())
	{
		l_o->setEnabled(true); // TODO we could disable the move actions one by one
	}
	enable_menu_actions();
}

void box_view::slot_add_item()
{
	mem_add_box *add = new mem_add_box(m_oControl);
	add->init(m_iId);
	add->box->m_iXX = m_oLastPoint.x();
	add->box->m_iYY = m_oLastPoint.y();
	add->apply();

	m_oItems.value(add->box->m_iId)->setSelected(true);
	// l_o->setPos(m_oLastPoint - QPointF(l_oRect.width()/2, l_oRect.height()/2)); // TODO
}

void box_view::slot_color()
{
	Q_ASSERT(m_oSelected.size() >= 1);
	QColor l_oColor = QColorDialog::getColor(l_oColor, this);
	if (!l_oColor.isValid()) return;
	foreach (QGraphicsItem *l_oItem, m_oSelected)
	{
		QAbstractGraphicsShapeItem* l_o = (QAbstractGraphicsShapeItem*) l_oItem;
		l_o->setBrush(l_oColor);
		if (l_oItem->type() == BOX_LINK_T)
		{
			box_link *l_oLink = (box_link*) l_oItem;
			QPen l_oPen = l_oLink->pen();
			l_oPen.setColor(l_oColor);
			l_oLink->setPen(l_oPen);
		}
		l_oItem->update();
	}
}

void box_view::slot_penstyle()
{
	int l_i = ((QAction*) QObject::sender())->data().toInt();
	foreach (QGraphicsItem* l_o, m_oSelected)
	{
		if (l_o->type() == BOX_LINK_T)
		{
			box_link *l_oLink = (box_link*) l_o;
			QPen l_oPen = l_oLink->pen();
			l_oPen.setStyle((Qt::PenStyle) l_i);
			l_oLink->setPen(l_oPen);
		}
	}

}

void box_view::slot_penwidth()
{
	int l_i = ((QAction*) QObject::sender())->data().toInt();
	foreach (QGraphicsItem* l_o, m_oSelected)
	{
		if (l_o->type() == BOX_LINK_T)
		{
			box_link *l_oLink = (box_link*) l_o;
			QPen l_oPen = l_oLink->pen();
			l_oPen.setWidth(l_i);
			l_oLink->setPen(l_oPen);
		}
	}
}

/*
void box_view::slot_edit()
{
	if (m_oSelected.size() == 1 && m_oSelected[0]->type() == BOX_ITEM_T)
	{
		box_item *l_o = (box_item*) m_oSelected[0];
		if (l_o->m_bEdit)
		{
			QFocusEvent l_oEv = QFocusEvent(QEvent::FocusOut);
			l_o->update();
		}
		else
		{
			l_o->update();
		}
	}
}
*/

void box_view::slot_toggle_edit()
{
	if (!hasFocus()) {
		return;
	}

	box_item* sel = NULL;
	foreach (QGraphicsItem *tmp, items()) {
		if (tmp->type() == BOX_ITEM_T && tmp->isSelected()) {
			if (sel) {
				sel = NULL;
				break;
			} else {
				sel = (box_item*) tmp;
			}
		}
	}

	if (sel) {
		if (sel->textInteractionFlags() & Qt::TextEditorInteraction) {
			sel->setTextInteractionFlags(Qt::NoTextInteraction);
			/*if (sel->toPlainText() == QObject::trUtf8("")) {
				sel->setPlainText(QObject::trUtf8("Empty"));
				sel->update(); // seems to update faster
				//sel->update_links();

			}*/
			if (sel->toPlainText() != m_oControl->m_oItems[m_iId]->m_oBoxes[sel->m_iId]->m_sText) {
				mem_edit_box *ed = new mem_edit_box(m_oControl, m_iId, sel->m_iId);
				ed->newText = sel->toPlainText();
				ed->apply();
			}
		} else {
			sel->setTextInteractionFlags(Qt::TextEditorInteraction);
			sel->setFocus();

			m_oAddItemAction->setEnabled(false);
			m_oDeleteAction->setEnabled(false);
			m_oCancelEditAction->setEnabled(true);

			/*
			m_oInsertSiblingAction->setEnabled(false);
			m_oNextRootAction->setEnabled(false);

			m_oMoveUpAction->setEnabled(false);
			m_oMoveDownAction->setEnabled(false);
			m_oMoveLeftAction->setEnabled(false);
			m_oMoveRightAction->setEnabled(false);
			m_oSelectUpAction->setEnabled(false);
			m_oSelectDownAction->setEnabled(false);
			m_oSelectLeftAction->setEnabled(false);
			m_oSelectRightAction->setEnabled(false);
			m_oControl->check_undo(false);
			*/
			return;
		}
	}

	m_oAddItemAction->setEnabled(true);
	m_oDeleteAction->setEnabled(true);
	m_oCancelEditAction->setEnabled(false);

	/*
	m_oInsertSiblingAction->setEnabled(true);
	m_oNextRootAction->setEnabled(true);

	m_oMoveUpAction->setEnabled(true);
	m_oMoveDownAction->setEnabled(true);
	m_oMoveLeftAction->setEnabled(true);
	m_oMoveRightAction->setEnabled(true);
	m_oSelectUpAction->setEnabled(true);
	m_oSelectDownAction->setEnabled(true);
	m_oSelectLeftAction->setEnabled(true);
	m_oSelectRightAction->setEnabled(true);
	m_oControl->check_undo(true);
	*/
}

void box_view::slot_cancel_edit()
{
	qDebug()<<"slot cancel edit";
	box_item* sel = NULL;
	foreach (QGraphicsItem *tmp, items()) {
		if (tmp->type() == BOX_ITEM_T && tmp->isSelected()) {
			if (sel) {
				sel = NULL;
				break;
			} else {
				sel = (box_item*) tmp;
			}
		}
	}

	if (sel && sel->textInteractionFlags() & Qt::TextEditorInteraction) {
		qDebug()<<"cancel edit!"<<sel;
		sel->setTextInteractionFlags(Qt::NoTextInteraction);
		sel->update_data();
	}

	m_oAddItemAction->setEnabled(true);
	m_oDeleteAction->setEnabled(true);
}

void box_view::slot_move_up()
{
	foreach (QGraphicsItem *l_oItem, m_oSelected)
	{
		l_oItem->setZValue(l_oItem->zValue()+1);
	}
}

void box_view::slot_move_down()
{
	foreach (QGraphicsItem *l_oItem, m_oSelected)
	{
		l_oItem->setZValue(l_oItem->zValue()-1);
	}
}

void box_view::from_string(const QString &i_s)
{
	box_reader l_oHandler(this);
	QXmlInputSource l_oSource;
	l_oSource.setData(i_s);
	QXmlSimpleReader l_oReader;
	l_oReader.setContentHandler(&l_oHandler);
	if (!l_oReader.parse(l_oSource))
	{
		//qDebug()<<"parse error!!";
		clear_diagram();
	}
}

void box_view::clear_diagram()
{
	foreach (box_link *l_o, m_oLinks) {
		scene()->removeItem(l_o);
		delete l_o;
	}
	m_oLinks.clear();
	foreach (box_item *l_o, m_oItems.values()) {
		scene()->removeItem(l_o);
		delete l_o;
	}
	m_oItems.clear();
}

/*
QString box_view::to_string()
{
	QStringList l;
	l<<QObject::trUtf8("<sem_diagram>");
	foreach (box_item *l_oBox, m_oItems.values())
	{
		l<<QObject::trUtf8("<box_item id=\"%1\"").arg(QString::number(l_oBox->m_iId));
		l<<QObject::trUtf8(" text=\"%1\"").arg(bind_node::protectXML(l_oBox->toPlainText()));
		l<<QObject::trUtf8(" c1=\"%1\" c2=\"%2\" c3=\"%3\" c4=\"%4\"").arg(
				QString::number(l_oBox->pos().x()),
				QString::number(l_oBox->pos().y()),
				QString::number(l_oBox->rect().width()),
				QString::number(l_oBox->rect().height()));
		l<<QObject::trUtf8(" col=\"%1\"").arg(l_oBox->brush().color().name());
		l<<QObject::trUtf8("/>\n");
	}

	foreach (box_link *l_oLink, m_oLinks)
	{
		l<<QObject::trUtf8("<box_link id=\"%1\" ").arg(QString::number(l_oLink->m_iId));
		l<<QObject::trUtf8(" p1=\"%1\" ").arg(QString::number(l_oLink->m_iParent));
		l<<QObject::trUtf8(" p2=\"%1\" ").arg(QString::number(l_oLink->m_iChild));
		l<<QObject::trUtf8(" wl=\"%1\" ").arg(QString::number(l_oLink->pen().width()));
		l<<QObject::trUtf8(" sl=\"%1\" ").arg(QString::number(l_oLink->pen().style()));

		if (l_oLink->m_oParent && l_oLink->m_oChild)
		{
			l<<QObject::trUtf8(" c1=\"%1\" ").arg(QString::number(l_oLink->m_oParent->m_iId));
			l<<QObject::trUtf8(" c2=\"%1\" ").arg(QString::number(l_oLink->m_oChild->m_iId));
		}
		l<<QObject::trUtf8(">\n");
		for (int i=0; i<l_oLink->m_oOffsets.size(); ++i)
		{
			l<<QObject::trUtf8("   <box_link_offset x=\"%1\" y=\"%2\"/>\n").arg(
				QString::number(l_oLink->m_oOffsets[i].x()),
				QString::number(l_oLink->m_oOffsets[i].y()));
		}
		l<<QObject::trUtf8("</box_link>\n");
	}

	l<<QObject::trUtf8("</sem_diagram>");

	return l.join("");
}*/

void box_view::check_canvas_size()
{
	/*
	QWidget *l_oW = viewport();
	QRect l_oRect = l_oW->rect();

	if (m_oItems.size() < 1)
	{
		scene()->setSceneRect(QRectF(mapToScene(l_oRect.topLeft()), mapToScene(l_oRect.bottomRight())));
		return;
	}

	QRectF l_oR2;

	qreal x, y, z, t;
	box_item *l_o = m_oItems[0];

	x = z = l_o->x() + l_o->rect().width()/2;
	y = t = l_o->y() + l_o->rect().height()/2;

	foreach (box_item *l_oItem, m_oItems)
	{
		if (l_oItem->x() < x) x = l_oItem->x();
		if (l_oItem->y() < y) y = l_oItem->y();
		if (l_oItem->x() > z) z = l_oItem->x() + l_oItem->rect().width();
		if (l_oItem->y() > t) t = l_oItem->y() + l_oItem->rect().height();
	}
	x -=100; y -= 100; z+=100, t += 100;

	l_oR2 = QRectF(QPointF(x, y), QPointF(z, t));
	l_oR2 = l_oR2.united(QRectF(mapToScene(l_oRect.topLeft()), mapToScene(l_oRect.bottomRight())));
	if (l_oR2 == sceneRect()) return;
	scene()->setSceneRect(l_oR2);
	*/
}

void box_view::focusInEvent(QFocusEvent *i_oEv)
{
	m_oControl->notify_focus(this);

	m_oDeleteAction->setEnabled(true);
	m_oEditAction->setEnabled(true);
	m_oAddItemAction->setEnabled(true);

	QGraphicsView::focusInEvent(i_oEv);
}

/*
void box_view::focusInEvent(QFocusEvent *i_oEv)
{
	enable_actions();
	QGraphicsView::focusInEvent(i_oEv);
}

void box_view::focusOutEvent(QFocusEvent *i_oEv)
{
	foreach (QAction* l_o, actions())
	{
		l_o->setEnabled(false);
	}

	if (m_oMenu->isVisible())
	{
		enable_menu_actions();
	}

	delete m_oCurrent;
	m_oCurrent = NULL;
	QGraphicsView::focusOutEvent(i_oEv);
}*/


void box_view::notify_add_box(int id, int box)
{
	box_item *l_o = new box_item(this, box);
	m_oItems[box] = l_o;
	l_o->update_data();
}

void box_view::notify_del_box(int id, int box)
{
	box_item *l_o = m_oItems.value(box);
        Q_ASSERT(l_o!=NULL);
        scene()->removeItem(l_o);
	m_oItems.remove(box);
	delete l_o;
}

void box_view::notify_link_box(int id, data_link* link)
{
	if (m_oCurrent)
	{
		m_oLinks.push_back(m_oCurrent);
		m_oCurrent->update();
		m_oCurrent = NULL;
	}
	else
	{
		box_link *l_o = new box_link(this);
		l_o->set_link(link);
		m_oCurrent->update_pos();
	}
}

void box_view::notify_unlink_box(int id, data_link* link)
{
	Q_ASSERT(!m_oCurrent);
	foreach (box_link *cur, m_oLinks) {
		if (cur->m_oLink == link)
		{
			delete cur;
			m_oLinks.removeAll(cur);
			break;
		}
	}
}


void box_view::wheelEvent(QWheelEvent *i_oEvent)
{
	QPointF l_o = mapToScene(i_oEvent->pos());
	qreal i_iScaleFactor = pow(2.0, i_oEvent->delta() / 440.0);
	qreal i_rFactor = matrix().scale(i_iScaleFactor, i_iScaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
	if (i_rFactor < 0.01 || i_rFactor > 1000) return;
	scale(i_iScaleFactor, i_iScaleFactor);
	centerOn(l_o + mapToScene(viewport()->rect().center()) - mapToScene(i_oEvent->pos()));
}

/*
void box_view::mouseDoubleClickEvent(QMouseEvent* i_oEv)
{
	if (i_oEv->button() != Qt::LeftButton) return;
	QPointF m_oLastPoint = mapToScene(i_oEv->pos());

	QGraphicsItem *l_oItem = itemAt(i_oEv->pos());
	if (l_oItem and l_oItem->type() == BOX_LINK_T)
	{
		box_link *l_oLink = (box_link*) l_oItem;
		m_oLinks.removeAll(l_oLink);
		m_oSelected.removeAll(l_oLink);
		delete l_oLink;
		return;
	}

}*/

void box_view::keyPressEvent(QKeyEvent *i_oEvent)
{
	QGraphicsView::keyPressEvent(i_oEvent);
	if (QApplication::keyboardModifiers() & Qt::ControlModifier) setCursor(Qt::CrossCursor);
}

void box_view::keyReleaseEvent(QKeyEvent *i_oEvent)
{
	QGraphicsView::keyReleaseEvent(i_oEvent);
	setCursor(Qt::ArrowCursor);
}

void box_view::mouseDoubleClickEvent(QMouseEvent* i_oEv)
{
	if (i_oEv->button() != Qt::LeftButton)
	{
		//qDebug()<<"left button on click, leaving";
		return;
	}

	QPointF m_oLastPoint = mapToScene(i_oEv->pos());
	QGraphicsItem *l_oItem = itemAt(i_oEv->pos());
	if (l_oItem && l_oItem->type() == BOX_LINK_T)
	{
		box_link *l_oLink = (box_link*) l_oItem;
		mem_unlink_box *rm = new mem_unlink_box(m_oControl, m_iId);
		rm->link = l_oLink->m_oLink;
		rm->apply();

		return;
	}

	if (i_oEv->modifiers() != Qt::ControlModifier) {
		//qDebug()<<"adding a box from double click";

		mem_add_box *add = new mem_add_box(m_oControl);
		add->init(m_iId);
		add->box->m_iXX = m_oLastPoint.x();
		add->box->m_iYY = m_oLastPoint.y();
		add->apply();

		m_oItems.value(add->box->m_iId)->setSelected(true);
	}
}

void box_view::mousePressEvent(QMouseEvent *i_oEv)
{
	if (i_oEv->button() == Qt::RightButton)
	{
		// select the item under the cursor if available and show the popup menu
		m_oLastPoint = mapToScene(i_oEv->pos());
		/*QGraphicsItem *l_oItem = scene()->itemAt(mapToScene(i_oEv->pos()));
		if (l_oItem && (l_oItem->type() == BOX_ITEM_T || l_oItem->type() == BOX_LINK_T))
		{
			if (!m_oSelected.contains(l_oItem))
			{
				deselect_all();
				add_select(l_oItem);
			}
		}
		else
		{
			deselect_all();
		}*/
		enable_menu_actions();
		m_oMenu->popup(i_oEv->globalPos());
		return;
	}

	m_bPressed = true;
	m_oLastMovePoint = mapToScene(i_oEv->pos());
	m_oLastPoint = mapToScene(i_oEv->pos());

	if (i_oEv->button() == Qt::MidButton)
	{
		m_oScrollPoint = i_oEv->pos();
		viewport()->setCursor(Qt::OpenHandCursor);
		m_bScroll = true;
		return;
	}

	QGraphicsItem *l_oItem = scene()->itemAt(mapToScene(i_oEv->pos()));
	if (l_oItem && l_oItem->type() == BOX_ITEM_T)
	{
		box_item *l_oRect = (box_item*) l_oItem;
		if (QApplication::keyboardModifiers() & Qt::ControlModifier)
		{
			if (m_oCurrent) return;
			m_oCurrent = new box_link(this);
			m_oCurrent->m_oParent = l_oRect;
			m_oCurrent->m_oChild = NULL;
			m_oCurrent->m_iParent = box_link::pos_inrect(l_oRect->rect(), l_oRect->pos() - m_oLastMovePoint);
			m_oCurrent->m_iChild = 0;
			m_oCurrent->update_pos();
			return;
		}

		QPointF l_o = l_oItem->pos();
		// TODO used by the handle for resizing the boxes - check that the click was on the handle
		if (m_oLastPoint.x() + m_oLastPoint.y() - l_o.x() - l_o.y() >
			l_oRect->rect().width() + l_oRect->rect().height() - 2*GRID_VALUE)
		{
			m_oOffsetPoint = QPointF(l_oRect->rect().width(), l_oRect->rect().height());
		}
		else
		{
			m_oOffsetPoint = QPointF(-1, -1);
		}
	}

	/*else if (l_oItem && l_oItem->type() == BOX_LINK_T)
	{
		box_link *l_oLink = (box_link*) l_oItem;
		if (i_oEv->modifiers() == Qt::ShiftModifier)
		{
			if (m_oSelected.contains(l_oLink))
			{
				rm_select(l_oLink);
			}
			else
			{
				add_select(l_oLink);
				l_oLink->m_iControlSegment = 0;
			}
		}
		else if (m_oSelected.contains(l_oLink) and m_oSelected.size() == 1)
		{
			// check if a control point is hit for changing the segment position
			bool l_b = false; // found
			for (int i=1; i<l_oLink->m_oGood.size() - 2; ++i)
			{
				QPointF l_o = QPointF((l_oLink->m_oGood[i].x()+l_oLink->m_oGood[i+1].x())/2,
						(l_oLink->m_oGood[i].y()+l_oLink->m_oGood[i+1].y())/2);
				QPointF l_oP = mapToScene(i_oEv->pos());
				l_oP.setX(l_o.x() - l_oP.x());
				l_oP.setY(l_o.y() - l_oP.y());
				qreal l_i = l_oP.x() * l_oP.x() + l_oP.y() * l_oP.y();
				if (l_i < 100) // if the click is close to the control point
				{
					l_b = true;
					l_oLink->m_iControlSegment = i;
					l_oLink->m_oControlPoint = l_o;
					break;
				}
			}

			// no control point was hit, usual behaviour
			if (!l_b)
			{
				deselect_all();
				add_select(l_oLink);
				l_oLink->m_iControlSegment = 0;
			}
		}
		else
		{
			deselect_all();
			add_select(l_oLink);
			l_oLink->m_iControlSegment = 0;
		}
	}
	else
	{
		deselect_all();
	}*/

	QGraphicsView::mousePressEvent(i_oEv);
}

void box_view::mouseMoveEvent(QMouseEvent *i_oEv)
{
	if (m_bScroll)
	{
		QScrollBar *l_h_bar = horizontalScrollBar();
		QScrollBar *l_v_bar = verticalScrollBar();

		QPoint l_o = i_oEv->pos() - m_oScrollPoint;

		l_h_bar->setValue(l_h_bar->value() + (isRightToLeft() ? l_o.x() : - l_o.x()));
		l_v_bar->setValue(l_v_bar->value() - l_o.y());

		m_oScrollPoint = i_oEv->pos();
		return;
	}

	if (!m_bPressed)
	{
		return;
	}

	QRectF l_oRect;
	l_oRect.setTopLeft(m_oLastMovePoint);
	m_oLastMovePoint = mapToScene(i_oEv->pos());
	l_oRect.setBottomRight(m_oLastMovePoint);

	if (m_oCurrent)
	{
		m_oCurrent->update_pos();
		return;
	}
	else
	{
		foreach (QGraphicsItem*tmp, scene()->selectedItems()) {
			if (tmp->type() == BOX_ITEM_T && tmp->isSelected()) {
				((box_item*) tmp)->update_links();
			}
		}
	}


	/*else
	{
		if (m_oSelected.size() == 1 and m_oSelected[0]->type() == BOX_LINK_T and
			((box_link*) m_oSelected[0])->m_iControlSegment)
		{
			box_link *l_oLink = (box_link*) m_oSelected[0];
			int i = l_oLink->m_iControlSegment;

			QPointF l_oP((l_oLink->m_oLst[i].x()+l_oLink->m_oLst[i+1].x())/2,
					(l_oLink->m_oLst[i].y()+l_oLink->m_oLst[i+1].y())/2);
			--i;

			QPointF l_oNew = m_oLastMovePoint - m_oLastPoint + l_oLink->m_oControlPoint - l_oP;
			l_oLink->m_oOffsets[i].setX((int) l_oNew.x());
			l_oLink->m_oOffsets[i].setY((int) l_oNew.y());
			l_oLink->update_ratio(); // no need to update_pos
			l_oLink->update();
		}
		else if (m_oSelected.size() == 1 and m_oSelected[0]->type() == BOX_ITEM_T and
			m_oOffsetPoint.x()>0 and m_oOffsetPoint.y()>0 )
		{
			QPointF l_o = m_oLastMovePoint - m_oLastPoint + m_oOffsetPoint;
			box_item *l_oItem = (box_item*) m_oSelected[0];
			l_o.setX(qMax(GRID_VALUE, l_o.x()));
			l_o.setY(qMax(GRID_VALUE, l_o.y()));
			l_oItem->setRect(0, 0, int_val(l_o.x()), int_val(l_o.y()));

			// then update the links
			foreach (box_link *l_oLink, m_oLinks)
			{
				// either the parent or the child
				if (l_oLink->m_oParent == l_oItem or l_oLink->m_oChild == l_oItem)
					l_oLink->update_pos();
			}
		}
		else
		{
			foreach (QGraphicsItem *l_oItem, m_oSelected)
			{
				if (l_oItem->type() == BOX_ITEM_T)
				{
					((box_item*) l_oItem)->moveBy(l_oRect.width(), l_oRect.height());
				}

				foreach (box_link *l_oLink, m_oLinks)
				{
					// either the parent or the child
					if (l_oLink->m_oParent == l_oItem or l_oLink->m_oChild == l_oItem)
						l_oLink->update_pos();
				}
			}
		}
	}*/
	//check_canvas_size();

	QGraphicsView::mouseMoveEvent(i_oEv);
}

void box_view::mouseReleaseEvent(QMouseEvent *i_oEv)
{
	if (m_bScroll)
	{
		QGraphicsView::mouseReleaseEvent(i_oEv);
		m_bPressed = false;
		m_bScroll = false;
		viewport()->setCursor(Qt::ArrowCursor);
		return;
	}

	m_bPressed = false;
	if (m_oCurrent)
	{
		box_item *l_oUnder = NULL;
		foreach (QGraphicsItem *l_oI1, scene()->items(m_oLastMovePoint))
		{
			if (l_oI1->type() == BOX_ITEM_T)
			{
				l_oUnder = (box_item*) l_oI1;
				break;
			}
		}

		if (l_oUnder)
		{
			if (!m_oCurrent->m_oChild)
				m_oCurrent->m_oChild = l_oUnder;
			else
				m_oCurrent->m_oParent = l_oUnder;

			//see notify_link_box();
			mem_link_box *ln = new mem_link_box(m_oControl, m_iId);
			ln->init(m_oCurrent->m_oParent->m_iId, 
				m_oCurrent->m_iParent, m_oCurrent->m_oChild->m_iId, m_oCurrent->m_iChild);
			ln->apply();
		}

		if (m_oCurrent)
		{
			delete m_oCurrent;
			m_oCurrent = NULL;
		}
	}
	else
	{
		QGraphicsView::mouseReleaseEvent(i_oEv);
	}
}

void box_view::notify_focus(void* ptr) {
	if (ptr != this) {
		m_oDeleteAction->setEnabled(false);
		m_oEditAction->setEnabled(false);
		m_oAddItemAction->setEnabled(false);
	}
}

#include "box_view.moc"

