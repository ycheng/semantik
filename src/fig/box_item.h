// Thomas Nagy 2007-2012 GPLV3


#ifndef BOX_ITEM_H
#define BOX_ITEM_H

#include <QGraphicsRectItem>
#include <QBrush>
#include "res:zable.h"
#include "ed:table.h"
#include "box_resize_point.h"
#include "box_chain.h"
#include "CON.h"
#include "con.h"

#define OFF 3

class QTextDocument;
class box_view;
class data_item;
class data_box;
class box_item : public QGraphicsRectItem, public connectable, public resizable, public editable
{
	public:
		box_item(box_view*, int i_iId);
		~box_item();

		void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *);

		QRectF rect() const { return boundingRect().translated(pos()).adjusted(JUST, JUST, -JUST, -JUST); };
		//void setRect(int, int, int, int) { }

		QTextDocument doc;

		box_view *m_oView;
		data_item *m_oItem;

		void mousePressEvent(QGraphicsSceneMouseEvent* e);
		void mouseMoveEvent(QGraphicsSceneMouseEvent *e);
		void mouseReleaseEvent(QGraphicsSceneMouseEvent* e);
		void properties();

		void update_data();
		void update_size();
		void update_links();

		QVariant itemChange(GraphicsItemChange i_oChange, const QVariant &i_oValue);

		int choose_position(const QPointF&p, int id=-1);
		QPoint get_point(int position);
		QPointF m_oLastPressPoint;

		int m_iWW;
		int m_iHH;
		bool m_bMoving;

		QPointF validate_point(box_resize_point *p, const QPointF & orig);
		void freeze(bool b);
		void commit_size(box_resize_point *p);
};

#endif // BOX_ITEM_H

