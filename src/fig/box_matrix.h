// Thomas Nagy 2007-2014 GPLV3

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
		void mouseMoveEvent(QGraphicsSceneMouseEvent *e);

		// not sure this is the best technique
		//QList<box_resize_point*> m_oRowPoints;
		//QList<box_resize_point*> m_oColPoints;
		//QVariant itemChange(GraphicsItemChange i_oChange, const QVariant &i_oValue);
		//void update_sizers();
		//int m_iLastStretch;
		//void freeze(bool b);
		//virtual QPointF validate_point(box_resize_point*, const QPointF& p) { return QPointF(p); }
		//virtual void commit_size(box_resize_point*) {}
};

#endif // BOX_MATRIX_H

