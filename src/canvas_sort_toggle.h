// Thomas Nagy 2007-2015 GPLV3

#ifndef CANVAS_SORT_TOGGLE_H
#define CANVAS_SORT_TOGGLE_H

#include <QGraphicsRectItem>
#include <QRectF>
#include <QPair>
#include <QSet>
#include <QPainterPath>
#include "con.h"
#include "canvas_view.h"

class canvas_sort_toggle : public QGraphicsRectItem
{
	public:
		canvas_sort_toggle(canvas_view *i_oGraphWidget);
		canvas_view *m_oParent;
		void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *);
}; 

#endif // CANVAS_SORT_TOGGLE_H

