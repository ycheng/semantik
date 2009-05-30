/// Thomas Nagy 2007-2009 GPLV3

#ifndef CANVAS_FLAG_H
#define CANVAS_FLAG_H

#include <QGraphicsRectItem> 
 #include <QList> 
  #include \
 	 "con.h" 

class QColor;
class QString;
class QGraphicsSceneMouseEvent;
class canvas_view;
class canvas_item;

class canvas_flag: public QGraphicsRectItem
{
	public:
		canvas_flag(canvas_view *i_oGraphWidget, canvas_item *i_oFrom);

		//enum { Type = gratype(797) };
		int type() const { return gratype(797); }
		canvas_view *m_oGraph;
		canvas_item *m_oItem;
		void paint(QPainter *i_oPainter, const QStyleOptionGraphicsItem *i_oOption, QWidget * i_oW);
};

#endif

