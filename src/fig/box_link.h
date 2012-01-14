// Thomas Nagy 2007-2012 GPLV3


#ifndef BOX_LINK_H
#define BOX_LINK_H

#include <QGraphicsRectItem>
#include <QRectF>
#include <QPair>
#include <QSet>
#include <QPainterPath>
#include "con.h"

class box_view;
class box_item;
class data_link;
class box_link : public QGraphicsRectItem
{
	public:
		box_link(box_view*);
		~box_link();

		int type() const { return BOX_LINK_T; }

		void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *);

		int m_iParent;
		int m_iChild;

		QPointF m_oMediatorPoint;

		data_link *m_oLink;

		box_item *m_oParent;
		box_item *m_oChild;

		box_view *m_oView;

		void update_pos();
		void draw_triangle(QPainter *i_oPainter, int i_iPosition, QPointF i_oP);

		QVariant itemChange(GraphicsItemChange i_oChange, const QVariant &i_oValue);

		QPainterPath shape() const;
		QPainterPath inner_shape;

		//int m_iId;

		QList<QPoint> m_oGood;
		QList<QPoint> m_oLst;

		void update_ratio();
		void set_link(data_link*);

		static int pos_inrect(const QRectF&, const QPointF&);
		int pos_heuristic(const QRectF&, int, const QPointF&);


		// This shows how to abuse the qt containers :-)
		QList<int> ver;
		QList<int> hor;
		QSet<QPair<int, int> > graph;
		QHash<QPair<int, int>, QPair<int, int> > prev;
		QHash<QPair<int, int>, int> dist;
		void set_rectangles(int ax1, int ax2, int ay1, int ay2, int ap, int bx1, int bx2, int by1, int by2, int bp);
		int may_use(QPair<int, int> cand, QPair<int, int> p, int ax1, int ax2, int ay1, int ay2, int bx1, int bx2, int by1, int by2);
}; 

#endif // BOX_LINK_H

