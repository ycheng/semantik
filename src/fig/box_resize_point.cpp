// Thomas Nagy 2007-2012 GPLV3

#include <QApplication>
#include <QAbstractTextDocumentLayout>
#include <QTextDocument>
#include <QTextDocumentFragment>
#include <QAbstractTextDocumentLayout>
#include <QTextList>
#include <QClipboard>
#include <QPainter>
#include <QtDebug>
#include <QAction>
#include <QTextDocument>
#include "box_resize_point.h"
#include "box_view.h"
#include "box_resize_point.h"
#include "mem_box.h"
 #include "box_link.h"
#include "data_item.h"
#include "sem_mediator.h"

#include <QGraphicsSceneMouseEvent>

#define PAD 1

box_resize_point::box_resize_point(box_view* i_oView, resizable *i_oR) : QGraphicsRectItem(), m_oParent(i_oR)
{
	setCacheMode(QGraphicsItem::DeviceCoordinateCache);
	m_oView = i_oView;
	m_bHeld = false;
	setZValue(110);
	m_bForced = false;
	m_iPosition = 0;
	setFlags(ItemIsMovable | ItemSendsGeometryChanges);
	i_oView->scene()->addItem(this);
}

void box_resize_point::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	painter->save();
	QRectF l_oRect = boundingRect().adjusted(PAD, PAD, -PAD, -PAD);

	QPen l_oPen = QPen(Qt::SolidLine);
	l_oPen.setColor(Qt::black);
	l_oPen.setCosmetic(false);
	l_oPen.setWidth(1);
	painter->setPen(l_oPen);
	painter->setBrush(QColor("#FFFF00"));
	painter->drawRect(l_oRect);
	painter->restore();
}

void box_resize_point::mousePressEvent(QGraphicsSceneMouseEvent* e) {
	e->accept();
	m_oParent->freeze(true);
	QGraphicsRectItem::mousePressEvent(e);
	m_bHeld = true;
}

void box_resize_point::mouseReleaseEvent(QGraphicsSceneMouseEvent* e) {
	e->accept();
	m_oParent->freeze(false);
	QGraphicsRectItem::mouseReleaseEvent(e);
	m_bHeld = false;
}

QVariant box_resize_point::itemChange(GraphicsItemChange i_oChange, const QVariant &i_oValue)
{
	if (scene())
	{
		if (i_oChange == ItemPositionChange && m_bHeld)
		{
			return m_oParent->validate_point(this, i_oValue.toPointF());
		}
	}
	return QGraphicsItem::itemChange(i_oChange, i_oValue);
}

