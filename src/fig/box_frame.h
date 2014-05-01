// Thomas Nagy 2007-2014 GPLV3

#ifndef BOX_FRAME_H
#define BOX_FRAME_H

#include <QGraphicsRectItem>
#include <QBrush>
#include "res:zable.h"
#include "ed:table.h"
#include "box_item.h"
#include "CON.h"
#include "con.h"

#define OFF 3

class box_frame : public box_item
{
	public:
		void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *);
		box_frame(box_view*, int i_iId);
};

#endif // BOX_FRAME_H

