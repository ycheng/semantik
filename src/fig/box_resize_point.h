// Thomas Nagy 2007-2012 GPLV3

#ifndef BOX_RESIZE_H
#define BOX_RESIZE_H

#include <QGraphicsRectItem>
#include <QBrush>
#include "CON.h"
#include "res:zable.h"
#include "con.h"

class box_view;
class data_item;
class data_box;
class box_link;
class resizable;
class box_resize_point : public QGraphicsRectItem
{
	public:
		box_resize_point(box_view*, resizable*);
		bool m_bMoveX;

		int m_iPosition;
		box_view *m_oView;
		resizable *m_oParent;

		void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *);
		QPoint m_oRealPosition;
		QVariant itemChange(GraphicsItemChange i_oChange, const QVariant &i_oValue);

		void mousePressEvent(QGraphicsSceneMouseEvent* e);
		void mouseReleaseEvent(QGraphicsSceneMouseEvent* e);

		bool m_bHeld;

	private:
		bool m_bChanged;
		bool m_bForced;

};

#endif // BOX_RESIZE_H

