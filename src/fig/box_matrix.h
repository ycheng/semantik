// Thomas Nagy 2007-2015 GPLV3

#ifndef BOX_MATRIX_H
#define BOX_MATRIX_H

#include <QGraphicsRectItem>
#include <QBrush>
#include "res:zable.h"
#include "ed:table.h"
#include "box_item.h"
#include <box_resize_point.h>
#include "CON.h"
#include "con.h"

#define OFF 3

class box_matrix : public box_item
{
	public:
		void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *);
		box_matrix(box_view*, int i_iId);
		void mousePressEvent(QGraphicsSceneMouseEvent* e);
		void mouseMoveEvent(QGraphicsSceneMouseEvent *e);
		void mouseReleaseEvent(QGraphicsSceneMouseEvent* e);
		void update_size();
		void properties();

		int m_iLastSize;
		int m_iMovingRow;
		int m_iMovingCol;
};

#endif // BOX_MATRIX_H

