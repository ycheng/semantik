// Thomas Nagy 2007-2014 GPLV3

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOption>
#include <QColor>
#include <QPen>
#include <QtDebug>

#include <math.h>

#include  "con.h"
#include "canvas_item.h"
#include  "canvas_sort.h"
#include "canvas_view.h"

canvas_sort::canvas_sort(canvas_view *i_oGraphWidget, canvas_item *i_oFrom)
	: QGraphicsEllipseItem(), m_oGraph(i_oGraphWidget)
{
	m_oFrom = i_oFrom;

	setZValue(123);
	//setRect(0, 0, 20, 20);
	//m_oColor = QColor("#FFFCD5");
	//m_oColor = QColor("#ABFBC7");

	hide();
	QPen l_oPen;
	l_oPen.setWidth(1);
	setPen(l_oPen);

	QRectF l_oRect;
	l_oRect.setTopLeft(QPointF(0, 0));
	l_oRect.setWidth(20);
	l_oRect.setHeight(20);
	setRect(l_oRect);

	setBrush(QColor(255, 255, 0));

	setPos(QPointF(-30, 0));
	setParentItem(m_oFrom);
}

void canvas_sort::paint(QPainter *i_oPainter, const QStyleOptionGraphicsItem *_option, QWidget * i_oW)
{
	if (m_oGraph->m_iSortCursor + 1 > m_oFrom->m_iNum)
	{
		i_oPainter->setBrush(QColor(0, 255, 0));
	}
	else
	{
		i_oPainter->setBrush(QColor(255, 255, 0));
	}

	i_oPainter->drawEllipse(boundingRect().adjusted(1, 1, -1, -1));
        i_oPainter->drawText(boundingRect().translated(0, 1), Qt::AlignCenter, QString::number(m_oFrom->m_iNum));
}

