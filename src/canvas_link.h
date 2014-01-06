/// Thomas Nagy 2007-2014 GPLV3

#ifndef CANVAS_LINK_H
#define CANVAS_LINK_H

#include  <QGraphicsPathItem>
#include  <QList>
#include  "con.h"

class QColor;
class QString;
class QGraphicsSceneMouseEvent;
class canvas_view;
class canvas_item;

class canvas_link: public QGraphicsPathItem
{
	public:
		canvas_link(canvas_view *i_oGraphWidget, canvas_item *i_oFrom, canvas_item * i_oTo);

		int type() const { return CANVAS_LINK_T; }

		QColor get_color() {return m_oColor;}
		void set_color(QColor c) {m_oColor=c;}

		void rm_link();

		void update_selected();

		bool m_bSel;

		canvas_view *m_oGraph;

		int m_iID;
		QColor m_oColor;
		QString m_sLabel;

		void update_pos();
		void update_spline();
		void update_triangle();

		canvas_item *m_oFrom;
		canvas_item *m_oTo;
};

#endif
