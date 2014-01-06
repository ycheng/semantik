// Thomas Nagy 2007-2014 GPLV3


#ifndef BOX_LINK_H
#define BOX_LINK_H

#include <QGraphicsRectItem>
#include <QRectF>
#include <QPair>
#include <QSet>
#include <QPainterPath>
#include "ed:table.h"
#include "con.h"

class box_view;
class box_item;
class data_link;
class box_control_point;
class box_link : public QGraphicsRectItem, public editable
{
	public:
		box_link(box_view*);
		~box_link();

		QPointF m_oMediatorPoint;

		data_link *m_oLink;
		data_link m_oInnerLink;

		box_view *m_oView;

		void update_pos();
		void draw_triangle(QPainter *i_oPainter, int i_iPosition, QPointF i_oP);
		void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *);

		QVariant itemChange(GraphicsItemChange i_oChange, const QVariant &i_oValue);

		QPainterPath shape() const;
		QPainterPath inner_shape;

		QList<box_control_point*> m_oControlPoints;
		box_control_point* m_oStartPoint;
		box_control_point* m_oEndPoint;

		QList<QPoint> m_oGood;
		QList<QPoint> m_oLst;

		void update_offset(const QPointF& i_oP, int i_iIdx);
		void update_ratio();

		void properties();

		bool m_bReentrantLock;

		// This shows how to abuse the qt containers :-)
		QList<int> ver;
		QList<int> hor;
		QSet<QPair<int, int> > graph;
		QHash<QPair<int, int>, QPair<int, int> > prev;
		QHash<QPair<int, int>, int> dist;
		void set_rectangles(int ax1, int ax2, int ay1, int ay2, int ap, QPoint&, int bx1, int bx2, int by1, int by2, int bp, QPoint&);
		int may_use(QPair<int, int> cand, QPair<int, int> p, int ax1, int ax2, int ay1, int ay2, int bx1, int bx2, int by1, int by2);
};

#endif // BOX_LINK_H

