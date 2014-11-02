// Thomas Nagy 2007-2014 GPLV3

#ifndef BOX_CLASS_H
#define BOX_CLASS_H

#include <QGraphicsRectItem>
#include <QBrush>
#include "res:zable.h"
#include "ed:table.h"
#include "box_item.h"
#include "CON.h"
#include "con.h"

#define OFF 3

class box_class : public box_item
{
	public:
		void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *);

		box_class(box_view*, int i_iId);
		QSizeF size();
		qreal minVisibility(const QFontMetricsF i_oFm);
		void mousePressEvent(QGraphicsSceneMouseEvent*);
		void update_size();

		void properties();
};

#endif // BOX_CLASS_H

