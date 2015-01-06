// Thomas Nagy 2007-2015 GPLV3

#include <QApplication>
#include <QPainter>
#include <QtDebug>
#include "CON.h"
#include "box_chain.h"
#include "box_view.h"
#include "data_item.h"
#include <box_control_point.h>

#define PAD 1

box_chain::box_chain(box_view* i_oParent) : QGraphicsRectItem()
{
	setVisible(false);
	setRect(0, 0, 10, 10);
}

void box_chain::paint(QPainter *i_oPainter, const QStyleOptionGraphicsItem *option, QWidget * i_oW)
{
        QPen l_oPen(Qt::SolidLine);
	l_oPen.setColor(Qt::black);
	l_oPen.setWidth(1);
	i_oPainter->setPen(l_oPen);

	i_oPainter->drawLine(QPoint(PAD, 5), QPoint(10-PAD, 5));
	i_oPainter->drawLine(QPoint(5, PAD), QPoint(5, 10-PAD));
}

