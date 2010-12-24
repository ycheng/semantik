// Thomas Nagy 2007-2011 GPLV3

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

void canvas_sort::paint(QPainter *_painter, const QStyleOptionGraphicsItem *_option, QWidget * i_oW)
{
        QGraphicsEllipseItem::paint(_painter, _option, i_oW);
        _painter->drawText(boundingRect().translated(0, 1), Qt::AlignCenter, m_oFrom->m_sNum);
}

