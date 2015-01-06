// Thomas Nagy 2007-2015 GPLV3

#ifndef CANVAS_CHAIN_H
#define CANVAS_CHAIN_H

#include <QGraphicsRectItem>
#include <QRectF>
#include <QPair>
#include <QSet>
#include <QPainterPath>
#include "con.h"
#include "canvas_view.h"

class canvas_chain : public QGraphicsRectItem
{
	public:
		canvas_chain(canvas_view *i_oGraphWidget);
		canvas_view *m_oParent;
		int type() const { return CANVAS_CHAIN_T; }
		void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *);
}; 

#endif // CANVAS_CHAIN_H

