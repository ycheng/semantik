// Thomas Nagy 2007-2015 GPLV3

#ifndef BOX_LABEL_H
#define BOX_LABEL_H

#include <QGraphicsRectItem>
#include <QBrush>
#include "res:zable.h"
#include "ed:table.h"
#include "box_item.h"
#include "CON.h"
#include "con.h"

#define OFF 3

class box_label : public box_item
{
	public:
		void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *);
		box_label(box_view*, int i_iId);
		QVariant itemChange(GraphicsItemChange i_oChange, const QVariant &i_oValue);
};

#endif // BOX_LABEL_H

