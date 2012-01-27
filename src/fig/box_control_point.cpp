// Thomas Nagy 2007-2012 GPLV3

#include <QApplication>
#include <QAbstractTextDocumentLayout>
#include <QTextDocument>
#include <QTextDocumentFragment>
#include <QAbstractTextDocumentLayout>
#include <QTextList>
#include <QClipboard>
#include <QPainter>
#include <QtDebug>
#include <QAction>
#include <QTextDocument>
#include "box_control_point.h"
#include "box_view.h"
#include "box_control_point.h"
 #include "box_link.h"
#include "data_item.h"
#include "sem_mediator.h"

#define PAD 1
#define CTRLSIZE 8
#define GRID 10

box_control_point::box_control_point(box_view* i_oParent) : QGraphicsRectItem(), m_oView(i_oParent)
{
	setRect(0, 0, CTRLSIZE, CTRLSIZE);
	is_connection = false;
	m_oLink = NULL;
	i_oParent->scene()->addItem(this);
	setCacheMode(QGraphicsItem::DeviceCoordinateCache);

	setZValue(110);
	setFlags(ItemIsMovable);
}

void box_control_point::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	painter->save();
	QRectF l_oRect = boundingRect().adjusted(PAD, PAD, -PAD, -PAD);

	QPen l_oPen = QPen(Qt::SolidLine);
	l_oPen.setColor(Qt::black);
	l_oPen.setCosmetic(false);
	l_oPen.setWidth(1);
	painter->setBrush(QColor("#FF8000")); // orangizer.com
	painter->drawRect(l_oRect);
	painter->restore();
}

/*
void box_control_point::mousePressEvent(QGraphicsSceneMouseEvent* e) {
	setZValue(100);
	QGraphicsRectItem::mousePressEvent(e);
}

void box_control_point::mouseReleaseEvent(QGraphicsSceneMouseEvent* e) {
	setZValue(99);
	QGraphicsRectItem::mouseReleaseEvent(e);
}*/

QVariant box_control_point::itemChange(GraphicsItemChange i_oChange, const QVariant &i_oValue)
{
	if (scene())
	{
		if (i_oChange == ItemPositionChange)
		{
			QPointF np = i_oValue.toPointF();
			np.setX(((int) np.x() / GRID) * GRID);
			np.setY(((int) np.y() / GRID) * GRID);
			return np;
		}
		else if (i_oChange == ItemPositionHasChanged)
		{
			//update_links();
		}
		else if (i_oChange == ItemSelectedHasChanged)
		{
			if (isSelected())
				setZValue(105);
			else
				setZValue(104);
		}
	}

	return QGraphicsItem::itemChange(i_oChange, i_oValue);
}

