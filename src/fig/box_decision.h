// Thomas Nagy 2007-2015 GPLV3

#ifndef BOX_DECISION_H
#define BOX_DECISION_H

#include <QGraphicsRectItem>
#include <QBrush>
#include "res:zable.h"
#include "ed:table.h"
#include "box_item.h"
#include "CON.h"
#include "con.h"

#define OFF 3

class box_decision : public box_item
{
	public:
		void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *);
		box_decision(box_view*, int i_iId);
		void update_size();
};

#endif // BOX_DECISION_H

