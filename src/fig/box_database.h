// Thomas Nagy 2007-2015 GPLV3

#ifndef BOX_DATABASE_H
#define BOX_DATABASE_H

#include <QGraphicsRectItem>
#include <QBrush>
#include "res:zable.h"
#include "ed:table.h"
#include "box_item.h"
#include "CON.h"
#include "con.h"

#define OFF 3

class box_database : public box_item
{
	public:
		void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *);
		box_database(box_view*, int i_iId);
};

#endif // BOX_ACTOR_H

