// Thomas Nagy 2007-2011 GPLV3


#ifndef BOX_ITEM_H
#define BOX_ITEM_H

#include <QGraphicsRectItem>
#include <QBrush>
#include "con.h"

#define OFF 3

class QTextDocument;
class box_view;
class data_item;
class data_box;
class box_item : public QGraphicsTextItem
{
	public:
		box_item(box_view*, int i_iId);
		~box_item();

		int type() const { return BOX_ITEM_T; }


		void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *);
		bool m_bSel;
		bool m_bEdit;

		qreal m_iXX;
		qreal m_iYY;

		QColor m_oColor;
		QColor m_oColorBackup;
		QString m_sLabel;

		QBrush brush() { return QBrush(); }
		//void move_by(qreal, qreal);
		//void set_pos(QPointF i_oP);
		//void set_pos(qreal i_iXX, qreal i_iYY);

		QRectF boundingRect() const;

		QRectF rect() const {return boundingRect(); }; // TODO
		void setRect(int, int, int, int) { }

		int m_iId;

		box_view *m_oView;
		data_box *m_oBox;
		data_item *m_oItem;

		void mousePressEvent(QGraphicsSceneMouseEvent* e);
		void mouseReleaseEvent(QGraphicsSceneMouseEvent* e);
		void keyPressEvent(QKeyEvent*);
		void keyReleaseEvent(QKeyEvent*);

		void update_data();
};

#endif // BOX_ITEM_H

