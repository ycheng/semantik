// Thomas Nagy 2007-2014 GPLV3

#ifndef BOX_MATRIX_H
#define BOX_MATRIX_H

#include <QGraphicsRectItem>
#include <QBrush>
#include "res:zable.h"
#include "ed:table.h"
#include "box_item.h"
#include "CON.h"
#include "con.h"

#define OFF 3

class box_matrix : public box_item
{
	public:
		void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *);
		box_matrix(box_view*, int i_iId);
		void mouseMoveEvent(QGraphicsSceneMouseEvent *e);
};

#endif // BOX_MATRIX_H

