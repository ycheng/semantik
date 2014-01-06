// Thomas Nagy 2007-2014 GPLV3

#ifndef BOX_CHAIN_H
#define BOX_CHAIN_H

#include <QGraphicsRectItem>
#include <QRectF>
#include <QPair>
#include <QSet>
#include <QPainterPath>
#include "con.h"

class box_view;
class box_item;
class data_link;
class box_control_point;
class box_chain : public QGraphicsRectItem
{
	public:
		box_chain(box_view*);

		void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *);
}; 

#endif // BOX_CHAIN_H

