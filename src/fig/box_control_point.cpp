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
	setRect(-CTRLSIZE/2., -CTRLSIZE/2., CTRLSIZE, CTRLSIZE);
	m_bIsSegment = true;
	m_oLink = NULL;
	i_oParent->scene()->addItem(this);
	setCacheMode(QGraphicsItem::DeviceCoordinateCache);

	setZValue(110);
	setFlags(ItemIsMovable | ItemSendsGeometryChanges);
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
			if (m_bIsSegment)
			{
				QPointF np = i_oValue.toPointF();
				if (m_bMoveX) {
					//np.setX(((int) np.x() / GRID) * GRID);
					np.setX(int_val2(np.x()));
					if (m_oLink->m_oLst.size() > m_iOffset)
						np.setY((m_oLink->m_oLst[m_iOffset + 1].y() + m_oLink->m_oLst[m_iOffset + 2].y()) / 2.);
				} else {
					//np.setY(((int) np.y() / GRID) * GRID);
					np.setY(int_val2(np.y()));
					if (m_oLink->m_oLst.size() > m_iOffset)
						np.setX((m_oLink->m_oLst[m_iOffset + 1].x() + m_oLink->m_oLst[m_iOffset + 2].x()) / 2.);
				}
				return np;
			}
		}
		else if (i_oChange == ItemPositionHasChanged)
		{
			//m_oLink->update_offset(pos(), m_iOffset);
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

void box_control_point::update_pos()
{
	Q_ASSERT(m_oLink);
	int i = m_iOffset;
	QPointF p(m_oLink->m_oLst.at(i+1));
	if (m_oLink->m_oLst.at(i+2).x() == p.x()) {
		p.setY((p.y() + m_oLink->m_oLst.at(i+2).y()) / 2);
		m_bMoveX = true;
	} else {
		p.setX((p.x() + m_oLink->m_oLst.at(i+2).x()) / 2);
		m_bMoveX = false;
	}
	setPos(p);
}

