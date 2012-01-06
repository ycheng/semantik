// Thomas Nagy 2007-2012 GPLV3

#ifndef CANVAS_SORT_H
#define CANVAS_SORT_H

#include  <QGraphicsEllipseItem>
#include  "con.h"
#include  <QList>

class QColor;
class QString;
class QGraphicsSceneMouseEvent;
class canvas_view;
class canvas_item;

class canvas_sort: public QGraphicsEllipseItem
{
	public:
		canvas_sort(canvas_view *i_oGraphWidget, canvas_item *i_oFrom);

		int type() const { return CANVAS_SORT_T; }
	//private:
		canvas_view *m_oGraph;

	//public:
		canvas_item *m_oFrom;
		void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);

};

#endif
