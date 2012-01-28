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
	m_bChanged = false;
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
	if (m_bIsSegment)
	{
		painter->setBrush(QColor("#FFFF00"));
	}
	else
	{
		painter->setBrush(QColor("#00FF00"));
	}
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
			if (m_bIsSegment)
			{
				if (m_bMoveX) {
					int dec = int_val2(np.x());
					np.setX(dec);
					if (m_oLink->m_oLst.size() > m_iOffset)
					{
						np.setY((m_oLink->m_oGood[m_iOffset + 1].y() + m_oLink->m_oGood[m_iOffset + 2].y()) / 2.);
						if (m_oLink->m_oGood[m_iOffset + 1].x() != dec)
						{
							m_oLink->m_oLink->m_oOffsets[m_iOffset].setX(dec - m_oLink->m_oLst[m_iOffset + 1].x());
							m_bChanged = true;
						}
					}
				} else {
					int dec = int_val2(np.y());
					np.setY(dec);
					if (m_oLink->m_oLst.size() > m_iOffset)
					{
						np.setX((m_oLink->m_oGood[m_iOffset + 1].x() + m_oLink->m_oGood[m_iOffset + 2].x()) / 2.);
						if (m_oLink->m_oGood[m_iOffset + 1].y() != dec)
						{
							m_oLink->m_oLink->m_oOffsets[m_iOffset].setY(dec - m_oLink->m_oLst[m_iOffset + 1].y());
							m_bChanged = true;
						}
					}
				}
				return np;
			}
			else
			{
				connectable *l_oUnder = NULL; // TODO do something with the z-index
				foreach (QGraphicsItem *l_oI1, scene()->items(np))
				{
					if (l_oUnder = dynamic_cast<connectable*>(l_oI1))
					{
						break;
					}
				}
				if (l_oUnder)
				{
					int pos = l_oUnder->choose_position(np);
					QPointF p = l_oUnder->get_point(pos);
					return p;
				}
				return np;
			}
		}
		else if (i_oChange == ItemPositionHasChanged)
		{
			if (m_bIsSegment && m_bChanged && m_oLink->m_oLst.size() > m_iOffset)
			{
				m_oLink->update_offset(pos(), m_iOffset);
				m_bChanged = false;
			}
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

void box_control_point::init_pos()
{
	Q_ASSERT(m_oLink);
	int i = m_iOffset;

	QPointF p(m_oLink->m_oGood.at(i+1));
	if (m_oLink->m_oGood.at(i+2).x() == p.x()) {
		p.setY((p.y() + m_oLink->m_oGood.at(i+2).y()) / 2);
		m_bMoveX = true;
	} else {
		p.setX((p.x() + m_oLink->m_oGood.at(i+2).x()) / 2);
		m_bMoveX = false;
	}
	setPos(p);
}

