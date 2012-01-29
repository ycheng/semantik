// Thomas Nagy 2007-2012 GPLV3


#ifndef BOX_DOT_H
#define BOX_DOT_H

#include <QGraphicsRectItem>
#include <QBrush>
#include "CON.h"
#include "con.h"

#define OFF 3

class QTextDocument;
class box_view;
class data_item;
class data_box;
class box_dot : public QGraphicsRectItem, public connectable
{
	public:
		box_dot(box_view*, int i_iId);
		~box_dot();

		void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *);

		//QRectF boundingRect() const;

		QRectF rect() const { return boundingRect().translated(pos()).adjusted(JUST, JUST, -JUST, -JUST); };
		//void setRect(int, int, int, int) { }

		box_view *m_oView;
		data_item *m_oItem;

		void mousePressEvent(QGraphicsSceneMouseEvent* e);
		void mouseReleaseEvent(QGraphicsSceneMouseEvent* e);
		//void keyPressEvent(QKeyEvent*);
		//void keyReleaseEvent(QKeyEvent*);

		void update_data();
		void update_links();

		QVariant itemChange(GraphicsItemChange i_oChange, const QVariant &i_oValue);
		int choose_position(const QPointF&p, int id=-1);
		QPoint get_point(int position);

};

#endif // BOX_DOT_H

