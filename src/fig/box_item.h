// Thomas Nagy 2007-2012 GPLV3


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

		qreal m_iXX;
		qreal m_iYY;

		QColor m_oColor;
		QColor m_oColorBackup;
		QString m_sLabel;

		QRectF boundingRect() const;

		QRectF rect() const { return boundingRect(); }; // TODO
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
		void update_links();

		QVariant itemChange(GraphicsItemChange i_oChange, const QVariant &i_oValue);
};

#endif // BOX_ITEM_H

