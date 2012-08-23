// Thomas Nagy 2007-2012 GPLV3

#ifndef BOX_NODE_H
#define BOX_NODE_H

#include <QGraphicsRectItem>
#include <QBrush>
#include "res:zable.h"
#include "ed:table.h"
#include "box_item.h"
#include "CON.h"
#include "con.h"

#define OFF 3

class box_node : public box_item
{
	public:
		void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *);
		box_node(box_view*, int i_iId);
		void update_size();

};

#endif // BOX_NODE_H

