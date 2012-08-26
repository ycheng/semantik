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
#include "mem_box.h"
 #include "box_link.h"
#include "data_item.h"
#include "sem_mediator.h"

#define PAD 1
#define CTRLSIZE 8

box_control_point::box_control_point(box_view* i_oParent) : QGraphicsRectItem(), m_oView(i_oParent)
{
	setRect(-CTRLSIZE/2., -CTRLSIZE/2., CTRLSIZE, CTRLSIZE);
	m_bIsSegment = true;
	m_oLink = NULL;
	i_oParent->scene()->addItem(this);
	setCacheMode(QGraphicsItem::DeviceCoordinateCache);

	setZValue(110);
	m_bChanged = false;
	m_bForced = false;
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
	painter->setPen(l_oPen);
	if (m_bIsSegment)
	{
		painter->setBrush(QColor("#FFFF00"));
	}
	else
	{
		if (
			(m_oLink->m_oStartPoint == this && m_oLink->m_oInnerLink.m_iParent != NO_ITEM) 
			||
			(m_oLink->m_oEndPoint == this && m_oLink->m_oInnerLink.m_iChild != NO_ITEM))
		{
			painter->setBrush(QColor(Qt::green));
		}
		else
			painter->setBrush(QColor(Qt::red));
	}
	painter->drawRect(l_oRect);
	painter->restore();
}

void box_control_point::mousePressEvent(QGraphicsSceneMouseEvent* e) {
	//qDebug()<<"FIXME mouse press event";
	QGraphicsRectItem::mousePressEvent(e);
}

void box_control_point::mouseReleaseEvent(QGraphicsSceneMouseEvent* e) {
	if (! m_oView->m_oCurrent)
	{
		if (!m_oLink->m_oLink->equals(m_oLink->m_oInnerLink))
		{
			mem_change_link_box* mem = new mem_change_link_box(m_oView->m_oMediator, m_oView->m_iId);
			mem->link = m_oLink->m_oLink;
			mem->prev.copy_from(*m_oLink->m_oLink);
			mem->next.copy_from(m_oLink->m_oInnerLink);
			mem->apply();
		}
	}
	QGraphicsRectItem::mouseReleaseEvent(e);
}

QVariant box_control_point::itemChange(GraphicsItemChange i_oChange, const QVariant &i_oValue)
{
	if (scene())
	{
		if (i_oChange == ItemPositionChange)
		{
			// TODO make sure items do not mess each other positions (crashes...)
			QPointF l_o = i_oValue.toPointF();
			QPoint np = QPoint(l_o.x(), l_o.y());

			if (m_oView->m_oCurrent && this == m_oLink->m_oEndPoint)
			{
				connectable *start = m_oView->m_oItems.value(m_oView->m_oCurrent->m_oInnerLink.m_iParent);
				QRectF r = start->rect();
				QPointF l_o1 = r.topLeft() - l_o + QPointF(r.width()/2, r.height()/2);
				double c_x = l_o1.x() * r.height();
				double c_y = l_o1.y() * r.width();

				int cand = 0;
				if (qAbs(c_x) > qAbs(c_y))
				{
					cand = (c_x > 0) ? data_link::WEST : data_link::EAST;
				}
				else
				{
					cand = (c_y > 0) ? data_link::NORTH : data_link::SOUTH;
				}
				m_oView->m_oCurrent->m_oInnerLink.m_iParentPos = cand + 64 * 500;
			}

			if (m_bIsSegment)
			{
				if (m_bMoveX) {
					int dec = int_val2(np.x());
					np.setX(dec);
					if (m_oLink->m_oLst.size() > m_iOffset + 1)
					{
						np.setY((m_oLink->m_oGood[m_iOffset + 1].y() + m_oLink->m_oGood[m_iOffset + 2].y()) / 2.);
						if (m_oLink->m_oGood[m_iOffset + 1].x() != dec)
						{
							m_oLink->m_oInnerLink.m_oOffsets[m_iOffset].setX(dec - m_oLink->m_oLst[m_iOffset + 1].x());
							m_bChanged = true;
						}
					}
				} else {
					int dec = int_val2(np.y());
					np.setY(dec);
					if (m_oLink->m_oLst.size() > m_iOffset + 1)
					{
						np.setX((m_oLink->m_oGood[m_iOffset + 1].x() + m_oLink->m_oGood[m_iOffset + 2].x()) / 2.);
						if (m_oLink->m_oGood[m_iOffset + 1].y() != dec)
						{
							m_oLink->m_oInnerLink.m_oOffsets[m_iOffset].setY(dec - m_oLink->m_oLst[m_iOffset + 1].y());
							m_bChanged = true;
						}
					}
				}
				return np;
			}
			else if (!m_bForced)
			{
				connectable *l_oUnder = NULL; // TODO do something with the z-index
				foreach (QGraphicsItem *l_oI1, scene()->items(np))
				{
					if ((l_oUnder = dynamic_cast<connectable*>(l_oI1)))
					{
						break;
					}
				}

				if (l_oUnder)
				{
					int l_iPosition = (int) l_oUnder->choose_position(l_o);
					if (l_oUnder)
					{
						if (m_oLink->m_oStartPoint == this)
						{
							if (l_iPosition == m_oLink->m_oInnerLink.m_iChildPos && m_oLink->m_oInnerLink.m_iChild == l_oUnder->m_iId)
								goto not_connected;
						}
						else if (m_oLink->m_oEndPoint == this)
						{
							if (l_iPosition == m_oLink->m_oInnerLink.m_iParentPos && m_oLink->m_oInnerLink.m_iParent == l_oUnder->m_iId)
								goto not_connected;
						}
					}

					np = l_oUnder->get_point(l_iPosition);
					if (m_oLink->m_oStartPoint == this)
					{
						m_oLink->m_oInnerLink.m_iParent     = l_oUnder->m_iId;
						m_oLink->m_oInnerLink.m_iParentPos  = l_iPosition;
						m_oLink->m_oInnerLink.m_oStartPoint = m_oRealPosition = np;
					}
					else if (m_oLink->m_oEndPoint == this)
					{
						m_oLink->m_oInnerLink.m_iChild    = l_oUnder->m_iId;
						m_oLink->m_oInnerLink.m_iChildPos = l_iPosition;
						m_oLink->m_oInnerLink.m_oEndPoint = m_oRealPosition = np;
					}
					update();
					return np;
				}

				not_connected:

				if (m_oLink->m_oStartPoint == this)
				{
					if (connectable* con = m_oView->m_oItems.value(m_oLink->m_oInnerLink.m_iChild))
					{
						QPointF bot = con->rect().bottomLeft();
						m_oLink->m_oInnerLink.m_iParentPos = con->pos_heuristic(np - QPoint(bot.x(), bot.y()), m_oLink->m_oInnerLink.m_iChildPos);
					}
					m_oLink->m_oInnerLink.m_iParent = NO_ITEM;
					m_oLink->m_oInnerLink.m_oStartPoint = m_oRealPosition = np;
				}
				else if (m_oLink->m_oEndPoint == this)
				{
					if (connectable* con = m_oView->m_oItems.value(m_oLink->m_oInnerLink.m_iParent))
					{
						QPointF bot = con->rect().bottomLeft();
						m_oLink->m_oInnerLink.m_iChildPos = con->pos_heuristic(np - QPoint(bot.x(), bot.y()), m_oLink->m_oInnerLink.m_iParentPos);
					}
					m_oLink->m_oInnerLink.m_iChild = NO_ITEM;
					m_oLink->m_oInnerLink.m_oEndPoint = m_oRealPosition = np;
				}

				update();
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
			else if (!m_bIsSegment && !m_bForced)
			{
				m_oLink->update_pos();
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

void box_control_point::force_position(const QPoint& i_oP)
{
	m_bForced = true;
	if (m_oRealPosition != i_oP)
	{
		m_oRealPosition = i_oP;
	}
	setPos(i_oP);
	m_bForced = false;
}

