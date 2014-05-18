// Thomas Nagy 2007-2014 GPLV3

#include <QApplication>
#include <QAbstractTextDocumentLayout>
#include <QTextDocument>
#include <QTextDocumentFragment>
#include <QTextList>
#include <QClipboard>
#include <QPainter>
#include <QtDebug>
#include <QAction>
#include <QTextDocument>
#include "box_class.h"
#include "data_item.h"
 #include "res:zable.h"
#include "box_view.h"
 #include "box_link.h"
#include "sem_mediator.h"
#include "mem_box.h"

#include <QFont>

#define PAD 2
#define MIN_FORK_SIZE 30

box_class::box_class(box_view* i_oParent, int i_iId) : QGraphicsRectItem(), connectable(), m_oView(i_oParent)
{
	m_iId = i_iId;

	m_oItem = m_oView->m_oMediator->m_oItems[m_oView->m_iId];
	m_oBox = m_oItem->m_oBoxes[m_iId];
	Q_ASSERT(m_oBox);

	i_oParent->scene()->addItem(this);
	m_oChain = new box_chain(i_oParent);
	m_oChain->setParentItem(this);

	setCacheMode(QGraphicsItem::DeviceCoordinateCache);

	update_size();
	setZValue(100);
	setFlags(ItemIsMovable | ItemIsSelectable | ItemSendsGeometryChanges);

	QSizeF l_o = size();
	setRect(0, 0, l_o.width(), l_o.height());
}

void box_class::paint(QPainter *i_oPainter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	i_oPainter->save();

	QRectF l_oRect = boundingRect().adjusted(PAD, PAD, -PAD, -PAD);

	QPen l_oPen = QPen(Qt::SolidLine);
	l_oPen.setColor(Qt::black);
	l_oPen.setCosmetic(false);
	l_oPen.setWidth(1);
	i_oPainter->setPen(l_oPen);

	// oh the text
	qreal l_oHpos = 0;
	{
		QFont l_oBoldFont(m_oView->font());
		l_oBoldFont.setBold(true);
		i_oPainter->setFont(l_oBoldFont);
		QFontMetricsF l_oFm(l_oBoldFont);

		QRectF l_oR = l_oFm.boundingRect(m_oBox->m_sText);

		l_oR.adjust(0, -1, 0, 1);
		l_oR.moveTo(0, 0);
		l_oR.setWidth(l_oRect.width());
		l_oR.translate(l_oRect.topLeft() + QPointF(0, l_oHpos));
		i_oPainter->drawText(l_oR, Qt::AlignCenter | Qt::TextSingleLine | Qt::AlignVCenter, m_oBox->m_sText);

		l_oHpos += l_oR.height();
	}

	QFont l_oNormalFont(m_oView->font());
	QFontMetricsF l_oNormalFm(l_oNormalFont);
	qreal l_iHVisibility = minVisibility(l_oNormalFm);

	QFont l_oItalicFont(m_oView->font());
	l_oItalicFont.setItalic(true);
	QFontMetricsF l_oItalicFm(l_oItalicFont);

	QFont l_oUnderlineFont(m_oView->font());
	l_oUnderlineFont.setUnderline(true);
	QFontMetricsF l_oUnderlineFm(l_oUnderlineFont);

	if (m_oBox->m_oAttributes.size() >= 0)
	{
		i_oPainter->drawLine(l_oRect.topLeft() + QPointF(0, l_oHpos), l_oRect.topRight() + QPointF(0, l_oHpos));
		l_oHpos += 1;
	}
	foreach (data_box_attribute *l_o, m_oBox->m_oAttributes) {
		QRectF l_oR;
		if (l_o->m_bStatic) {
			l_oR = l_oUnderlineFm.boundingRect(l_o->m_sText);
			i_oPainter->setFont(l_oUnderlineFont);
		} else {
			l_oR = l_oNormalFm.boundingRect(l_o->m_sText);
			i_oPainter->setFont(l_oNormalFont);
		}

		l_oR.moveTo(0, 0);
		l_oR.setWidth(l_oRect.width());
		qreal l_fOff = 2*PAD + l_iHVisibility;
		l_oR.setWidth(l_oR.width() - l_fOff);
		l_oR.translate(l_oRect.topLeft() + QPointF(l_fOff, l_oHpos));

		i_oPainter->drawText(l_oR, Qt::AlignLeft | Qt::TextSingleLine | Qt::AlignTop, l_o->m_sText);

		QString l_sVis;
		if (l_o->m_oVisibility == visibility::PUBLIC) {
			l_sVis = "+";
		} else if (l_o->m_oVisibility == visibility::PROTECTED) {
			l_sVis = "#";
		} else if (l_o->m_oVisibility == visibility::PRIVATE) {
			l_sVis = "-";
		} else if (l_o->m_oVisibility == visibility::PACKAGE) {
			l_sVis = "~";
		} else if (l_o->m_oVisibility == visibility::DERIVED) {
			l_sVis = "/";
		} else {
			Q_ASSERT("invalid visibility " + l_o->m_oVisibility);
		}

		l_oR.translate(QPointF(PAD - l_fOff, 0));
		i_oPainter->setFont(l_oNormalFont);
		i_oPainter->drawText(l_oR, Qt::AlignLeft | Qt::TextSingleLine | Qt::AlignTop, l_sVis);
		l_oHpos += l_oR.height();
	}

	if (m_oBox->m_oMethods.size() >= 0)
	{
		i_oPainter->drawLine(l_oRect.topLeft() + QPointF(0, l_oHpos), l_oRect.topRight() + QPointF(0, l_oHpos));
		l_oHpos += 1;
	}
	foreach (data_box_method *l_o, m_oBox->m_oMethods) {

		QRectF l_oR;
		if (l_o->m_bAbstract) {
			l_oR = l_oItalicFm.boundingRect(l_o->m_sText);
			i_oPainter->setFont(l_oItalicFont);
		} else if (l_o->m_bStatic) {
			l_oR = l_oUnderlineFm.boundingRect(l_o->m_sText);
			i_oPainter->setFont(l_oUnderlineFont);
		} else {
			l_oR = l_oNormalFm.boundingRect(l_o->m_sText);
			i_oPainter->setFont(l_oNormalFont);
		}

		l_oR.moveTo(0, 0);
		l_oR.setWidth(l_oRect.width());
		qreal l_fOff = 2*PAD + l_iHVisibility;
		l_oR.setWidth(l_oR.width() - l_fOff);
		l_oR.translate(l_oRect.topLeft() + QPointF(l_fOff, l_oHpos));

		i_oPainter->drawText(l_oR, Qt::AlignLeft | Qt::TextSingleLine | Qt::AlignTop, l_o->m_sText);

		QString l_sVis;
		if (l_o->m_oVisibility == visibility::PUBLIC) {
			l_sVis = "+";
		} else if (l_o->m_oVisibility == visibility::PROTECTED) {
			l_sVis = "#";
		} else if (l_o->m_oVisibility == visibility::PRIVATE) {
			l_sVis = "-";
		} else if (l_o->m_oVisibility == visibility::PACKAGE) {
			l_sVis = "~";
		} else if (l_o->m_oVisibility == visibility::DERIVED) {
			l_sVis = "/";
		} else {
			Q_ASSERT("invalid visibility " + l_o->m_oVisibility);
		}

		l_oR.translate(QPointF(PAD - l_fOff, 0));
		//l_oR.translate(l_oRect.topLeft() + QPointF(PAD, l_oHpos));
		i_oPainter->setFont(l_oNormalFont);
		i_oPainter->drawText(l_oR, Qt::AlignLeft | Qt::TextSingleLine | Qt::AlignTop, l_sVis);
		l_oHpos += l_oR.height();
	}

	i_oPainter->drawRect(l_oRect);
	/*if (isSelected())
	{
		i_oPainter->setBrush(QColor("#FFFF00"));
		QRectF l_oR2(m_iWW - 8, m_iHH - 8, 6, 6);
		i_oPainter->drawRect(l_oR2);
	}*/
	i_oPainter->restore();
}

qreal box_class::minVisibility(const QFontMetricsF i_oFm)
{
	qreal l_iHVisibility = 0;
	if (i_oFm.width("+") > l_iHVisibility) {
		l_iHVisibility = i_oFm.width("+");
	}
	if (i_oFm.width("-") > l_iHVisibility) {
		l_iHVisibility = i_oFm.width("-");
	}
	if (i_oFm.width("#") > l_iHVisibility) {
		l_iHVisibility = i_oFm.width("#");
	}
	if (i_oFm.width("~") > l_iHVisibility) {
		l_iHVisibility = i_oFm.width("~");
	}
	if (i_oFm.width("/") > l_iHVisibility) {
		l_iHVisibility = i_oFm.width("/");
	}
	return l_iHVisibility;
}

QSizeF box_class::size() {
	QSizeF l_oRet;

	QFont l_oNormalFont(m_oView->font());
	QFontMetricsF l_oNormalFm(l_oNormalFont);
	qreal l_iHVisibility = minVisibility(l_oNormalFm);

	QFont l_oItalicFont(m_oView->font());
	l_oItalicFont.setItalic(true);
	QFontMetricsF l_oItalicFm(l_oItalicFont);

	QFont l_oUnderlineFont(m_oView->font());
	l_oUnderlineFont.setUnderline(true);
	QFontMetricsF l_oUnderlineFm(l_oUnderlineFont);


	qreal l_iWW = 0, l_iHH = 1;

	if (m_oBox->m_oMethods.size() >= 0)
	{
		l_iHH += 1;
	}
	foreach (data_box_method *l_o, m_oBox->m_oMethods) {
		QRectF l_oR;
		if (l_o->m_bAbstract) {
			l_oR = l_oItalicFm.boundingRect(l_o->m_sText);
		} else if (l_o->m_bStatic) {
			l_oR = l_oUnderlineFm.boundingRect(l_o->m_sText);
		} else {
			l_oR = l_oNormalFm.boundingRect(l_o->m_sText);
		}

		l_iWW = qMax(l_oR.width(), l_iWW);
		l_iHH += l_oR.height() + PAD;
	}

	if (m_oBox->m_oAttributes.size() >= 0)
	{
		l_iHH += 1;
	}
	foreach (data_box_attribute *l_o, m_oBox->m_oAttributes) {
		QRectF l_oR;
		if (l_o->m_bStatic) {
			l_oR = l_oUnderlineFm.boundingRect(l_o->m_sText);
		} else {
			l_oR = l_oNormalFm.boundingRect(l_o->m_sText);
		}

		l_iWW = qMax(l_oR.width(), l_iWW);
		l_iHH += l_oR.height() + PAD;
	}

	l_iWW += 3 * PAD + l_iHVisibility;

	l_oNormalFont.setBold(true);
	QFontMetricsF l_oFm(l_oNormalFont);
	{
		QRectF l_oR = l_oFm.boundingRect(m_oBox->m_sText);
		l_oR.adjust(0, -1, 0, 1);
		l_iWW = qMax(l_oR.width() +  2 * PAD, l_iWW);
		l_iHH += l_oR.height() + PAD;
	}

	return QSizeF(l_iWW, l_iHH);
}

