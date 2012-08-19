// Thomas Nagy 2007-2012 GPLV3

#ifndef BOX_FORK_H
#define BOX_FORK_H

#include <QGraphicsRectItem>
#include <QBrush>
#include "CON.h"
#include "res:zable.h"
#include "con.h"

#define OFF 3
#define FORK_LENGTH 160
#define FORK_WIDTH 8

class QTextDocument;
class box_view;
class data_item;
class data_box;
class box_fork : public QGraphicsRectItem, public connectable, public resizable
{
	public:
		box_fork(box_view*, int i_iId);
		~box_fork();

		void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *);

		QRectF rect() const { return boundingRect().translated(pos()).adjusted(JUST, JUST, -JUST, -JUST); };

		box_view *m_oView;
		data_item *m_oItem;

		void mousePressEvent(QGraphicsSceneMouseEvent* e);
		void mouseReleaseEvent(QGraphicsSceneMouseEvent* e);

		void update_data();
		void update_links();

		box_resize_point *m_oTop;
		box_resize_point *m_oDown;
		box_resize_point *m_oLeft;
		box_resize_point *m_oRight;

		QPointF validate_point(box_resize_point *p, const QPointF & orig);

		QVariant itemChange(GraphicsItemChange i_oChange, const QVariant &i_oValue);
		int choose_position(const QPointF&p, int id=-1);
		QPoint get_point(int position);
		void freeze(bool b);
};

#endif // BOX_FORK_H

