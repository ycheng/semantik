// Thomas Nagy 2007-2014 GPLV3

#include <QApplication>
#include <QPainter>
#include <QtDebug>
#include "CON.h"
#include "canvas_sort_toggle.h"
#include "box_view.h"
#include "data_item.h"
#include <box_control_point.h>

#define PAD 1

canvas_sort_toggle::canvas_sort_toggle(canvas_view* i_oParent) : QGraphicsRectItem()
{
	m_oParent = i_oParent;
	setVisible(false);
	setRect(0, 0, 10, 10);
}

void canvas_sort_toggle::paint(QPainter *i_oPainter, const QStyleOptionGraphicsItem *option, QWidget * i_oW)
{
        QPen l_oPen(Qt::SolidLine);
	l_oPen.setColor(Qt::black);
	l_oPen.setWidth(1);
	i_oPainter->setPen(l_oPen);

	i_oPainter->setBrush(Qt::yellow); // yolo

	i_oPainter->drawEllipse(boundingRect().adjusted(PAD, PAD, -PAD, -PAD));
}

