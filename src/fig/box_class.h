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

class box_class : public QGraphicsRectItem, public connectable
{
	public:
		void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *);
		box_class(box_view*, int i_iId);
		QSizeF size();
		qreal minVisibility(const QFontMetricsF i_oFm);

		box_view *m_oView;
		data_item *m_oItem;


};

#endif // BOX_CLASS_H

