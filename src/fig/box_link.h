// Thomas Nagy 2007-2009 GPLV3


#ifndef BOX_LINK_H
#define BOX_LINK_H

#include <QGraphicsRectItem>
#include <QRectF>
#include "con.h"

class box_view;
class box_item;
class box_link : public QGraphicsRectItem
{
	public:
		box_link(box_view*);
		~box_link();

		int type() const { return gratype(39); }

		void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *);
		bool m_bSel;

		int m_iParent;
		int m_iChild;
		int m_iParentPos;
		int m_iChildPos;


		int m_iControlSegment;
		QPointF m_oControlPoint;

		box_item *m_oParent;
		box_item *m_oChild;

		box_view *m_oView;

		void update_pos();

		void draw_triangle(QPainter *i_oPainter, int i_iPosition, QPointF i_oP);

		bool contains(const QPointF& i_oP) CONST;

		int m_iId;

		QList<QPoint> m_oGood;
		QList<QPoint> m_oLst;
		QList<QPoint> m_oOffsets; // this is the list we will keep

		void update_ratio();

		static int pos_inrect(const QRectF&, const QPointF&);
		int pos_heuristic(const QRectF&, int, const QPointF&);
};

#endif // BOX_LINK_H

