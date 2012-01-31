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

#define PAD 1
#define CTRLSIZE 8
#define GRID 10

box_resize_point::box_resize_point(box_view* i_oParent) : QGraphicsRectItem(), m_oView(i_oParent)
{
	setRect(-CTRLSIZE/2., -CTRLSIZE/2., CTRLSIZE, CTRLSIZE);
	i_oParent->scene()->addItem(this);
	setCacheMode(QGraphicsItem::DeviceCoordinateCache);

	setZValue(110);
	m_bForced = false;
	//m_oBox = NULL;
	m_iPosition = 0;
	setFlags(ItemIsMovable | ItemSendsGeometryChanges);
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
	QGraphicsRectItem::mousePressEvent(e);
}

void box_resize_point::mouseReleaseEvent(QGraphicsSceneMouseEvent* e) {
	// TODO change the size effectively
	/*
	if (!m_oLink->m_oLink->equals(m_oLink->m_oInnerLink))
	{
		mem_change_link_box* mem = new mem_change_link_box(m_oView->m_oMediator, m_oView->m_iId);
		mem->link = m_oLink->m_oLink;
		mem->prev.copy_from(*m_oLink->m_oLink);
		mem->next.copy_from(m_oLink->m_oInnerLink);
		mem->apply();
	}*/
	QGraphicsRectItem::mouseReleaseEvent(e);
}

QVariant box_resize_point::itemChange(GraphicsItemChange i_oChange, const QVariant &i_oValue)
{
	if (scene())
	{
		if (i_oChange == ItemPositionChange)
		{
			QPointF l_o = i_oValue.toPointF();
			QPoint np = QPoint(int_val2(l_o.x()), int_val2(l_o.y()));
			return np;
		}
		else if (i_oChange == ItemPositionHasChanged)
		{
		}
		else if (i_oChange == ItemSelectedHasChanged)
		{
			if (isSelected())
				setZValue(105);
			else
				setZValue(104);
		}
	}
	return QGraphicsItem::itemChange(i_oChange, i_oValue);
}

void box_resize_point::force_position(const QPoint& i_oP)
{
	m_bForced = true;
	if (m_oRealPosition != i_oP)
	{
		m_oRealPosition = i_oP;
	}
	setPos(i_oP);
	m_bForced = false;
}

