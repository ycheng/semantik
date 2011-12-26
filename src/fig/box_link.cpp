// Thomas Nagy 2007-2011 GPLV3


#include <QApplication>
#include <QPainter>
#include <QtDebug>
#include "box_link.h"
#include "box_view.h"
#include "box_item.h"

#define pad 25

box_link::box_link(box_view* i_oParent) : QGraphicsRectItem()
{
	m_bSel = false;
	/*setBrush(QColor(0, 255, 0, 160));
	setPen(QColor(Qt::black));*/

	m_iId = i_oParent->next_id();

        QPen l_oPen;
        l_oPen.setWidth(1);
	l_oPen.setColor(QColor(Qt::black));
        setPen(l_oPen);

	setBrush(QColor(Qt::black));

	i_oParent->scene()->addItem(this);
	setZValue(60);

	m_iParent = 0;
	m_iChild = 0;

	m_oParent = NULL;
	m_oChild = NULL;
	m_oView = i_oParent;

	// control segment: used to resize the segments
	m_iControlSegment = 0;
}

box_link::~box_link()
{

}

void box_link::paint(QPainter *i_oPainter, const QStyleOptionGraphicsItem *option, QWidget * i_oW)
{
	//i_oPainter->setPen(pen());
	//QGraphicsRectItem::paint(i_oPainter, option, i_oW);

	i_oPainter->setPen(pen());
	i_oPainter->setBrush(brush());

	for (int i=0; i<m_oGood.size() - 1; ++i)
	{
		QLineF l_oLine(m_oGood[i].x(), m_oGood[i].y(), m_oGood[i+1].x(), m_oGood[i+1].y());
		i_oPainter->drawLine(l_oLine);
		//qDebug()<<m_oGood[i].x()<<m_oGood[i].y()<<m_oGood[i+1].x()<<m_oGood[i+1].y();
	}
	QPen l_oPen = pen();
	l_oPen.setStyle(Qt::SolidLine);
	i_oPainter->setPen(l_oPen);

	//draw_triangle(i_oPainter, m_iParent, m_oGood[0]);
	draw_triangle(i_oPainter, m_iChild, m_oGood[m_oGood.size()-1]);

	l_oPen.setWidth(1);
	i_oPainter->setPen(l_oPen);

	QPointF l_oOffset(3, 3);
	if (!m_oParent && m_oChild)
	{
		i_oPainter->setBrush(QColor(255, 255, 0));
		QPointF l_o = m_oGood[m_oGood.size()-1];
		i_oPainter->drawEllipse(QRectF(l_o + l_oOffset, l_o - l_oOffset));
		box_item *l_oUnder = NULL;
		foreach (QGraphicsItem *l_oI1, scene()->items(m_oView->m_oLastMovePoint))
		{
			if (l_oI1->type() == BOX_ITEM_T)
			{
				l_oUnder = (box_item*) l_oI1;
				break;
			}
		}
		if (l_oUnder)
		{
			l_o = m_oGood[0];
			i_oPainter->drawEllipse(QRectF(l_o + l_oOffset, l_o - l_oOffset));
		}
	}
	else if (!m_oChild && m_oParent)
	{
		i_oPainter->setBrush(QColor(255, 255, 0));
		QPointF l_o = m_oGood[0];
		i_oPainter->drawEllipse(QRectF(l_o + l_oOffset, l_o - l_oOffset));
		box_item *l_oUnder = NULL;
		foreach (QGraphicsItem *l_oI1, scene()->items(m_oView->m_oLastMovePoint))
		{
			if (l_oI1->type() == BOX_ITEM_T)
			{
				l_oUnder = (box_item*) l_oI1;
				break;
			}
		}
		if (l_oUnder)
		{
			l_o = m_oGood[m_oGood.size()-1];
			i_oPainter->drawEllipse(QRectF(l_o + l_oOffset, l_o - l_oOffset));
		}
	}
	else if (m_oChild && m_oParent && m_oParent->m_oView->m_oSelected.contains(this))
	{
		// if the link is selected
		i_oPainter->setBrush(QColor(255, 255, 0));
		QPointF l_o = m_oGood[0];
		i_oPainter->drawEllipse(QRectF(l_o + l_oOffset, l_o - l_oOffset));
		l_o = m_oGood[m_oGood.size()-1];
		i_oPainter->drawEllipse(QRectF(l_o + l_oOffset, l_o - l_oOffset));

		// show the control points on all the segments except the borders
		for (int i=1; i<m_oGood.size() - 2; ++i)
		{
			QPointF l_o((m_oGood[i].x() + m_oGood[i+1].x())/2, (m_oGood[i].y() + m_oGood[i+1].y())/2);
			i_oPainter->drawEllipse(QRectF(l_o + l_oOffset, l_o - l_oOffset));
		}
	}
}


#define xw 3.
#define yw 8.

void box_link::draw_triangle(QPainter *i_oPainter, int i_iPos, QPointF i_oP)
{
	QPolygonF l_oPol;
	l_oPol<<i_oP;
	switch (i_iPos)
	{
		case 0:
			{
				l_oPol<<i_oP+QPointF(-xw, -yw);
				l_oPol<<i_oP+QPointF( xw, -yw);
			}
			break;
		case 1:
			{
				l_oPol<<i_oP+QPointF(-yw, -xw);
				l_oPol<<i_oP+QPointF(-yw,  xw);
			}
			break;
		case 2:
			{
                                l_oPol<<i_oP+QPointF(-xw, yw);
                                l_oPol<<i_oP+QPointF( xw, yw);
                        }
			break;
		case 3:
			{
                                l_oPol<<i_oP+QPointF(yw, -xw);
                                l_oPol<<i_oP+QPointF(yw,  xw);
                        }
			break;
		default:
			break;
	}
	i_oPainter->drawPolygon(l_oPol);
}

int box_link::pos_heuristic(const QRectF & i_oR, int i_iPos, const QPointF & i_oP)
{
	switch (i_iPos)
	{
		case 0:
			return (i_oP.y() > 0) ? 0 : 2;
			break;
		case 1:
			return (i_oP.x() < 0) ? 3 : 1;
			break;
		case 2:
			return (i_oP.y() < i_oR.height()) ? 2 : 0;
			break;
		case 3:
			return (i_oP.x() > i_oR.width()) ? 1 : 3;
			break;
		default:
			break;
	}
	return 2;
}

int box_link::pos_inrect(const QRectF & i_oR, const QPointF & i_oP)
{
	QPointF l_o = i_oP + QPointF(i_oR.width()/2, i_oR.height()/2);
	double c_x = l_o.x() * i_oR.height();
	double c_y = l_o.y() * i_oR.width();
	if (qAbs(c_x) > qAbs(c_y))
	{
		return (c_x > 0) ? 1 : 3;
	}
	else
	{
		return (c_y > 0) ? 0 : 2;
	}
	return 0;
}

void box_link::update_pos()
{
	//if (!m_oParent || !m_oChild) return;
	// now we are about certain we can work

	QRectF l_oR1, l_oR2;
	QPointF l_oP = m_oView->m_oLastMovePoint;


	//int ax1, ax2, ay1, ay2, ap, bx1, bx2, by1, by2, bp;

	// FIXME ITA  find the box_item under the pointer
	box_item *l_oUnder = NULL;
        foreach (QGraphicsItem *l_oI1, scene()->items(m_oView->m_oLastMovePoint))
        {
                if (l_oI1->type() == BOX_ITEM_T)
                {
                        l_oUnder = (box_item*) l_oI1;
			break;
                }
        }

	if (m_oParent)
	{
		l_oR1 = m_oParent->rect().translated(m_oParent->pos());
	}
	else if (l_oUnder)
	{
		l_oR1 = l_oUnder->rect().translated(l_oUnder->pos());
		m_iParent = pos_inrect(l_oUnder->rect(), l_oUnder->pos() - l_oP);
	}

	if (m_oChild)
	{
		l_oR2 = m_oChild->rect().translated(m_oChild->pos());
	}
	else if (l_oUnder)
	{
		l_oR2 = l_oUnder->rect().translated(l_oUnder->pos());
		m_iChild = pos_inrect(l_oUnder->rect(), l_oUnder->pos() - l_oP);
	}

	if (!l_oUnder && (!m_oParent || !m_oChild))
	{
		QRectF l_o = QRectF(l_oP - QPointF(1, 1), l_oP + QPointF(1, 1));

		if (!m_oParent)
		{
			if (!l_oUnder) m_iParent = pos_heuristic(l_oR2, m_iChild, l_oP - m_oChild->pos());
			l_oR1 = l_o;
		}
		if (!m_oChild)
		{
			if (!l_oUnder) m_iChild = pos_heuristic(l_oR1, m_iParent, l_oP - m_oParent->pos());
			l_oR2 = l_o;
		}
	}


	int ax1 = (int) l_oR1.x();
	int ax2 = (int) (l_oR1.x()+l_oR1.width());
	int ay1 = (int) l_oR1.y();
	int ay2 = (int) (l_oR1.y()+l_oR1.height());

	int bx1 = (int) l_oR2.x();
	int bx2 = (int) (l_oR2.x()+l_oR2.width());
	int by1 = (int) l_oR2.y();
	int by2 = (int) (l_oR2.y()+l_oR2.height());

	int low_x = qMin(ax1, bx1) - 20;
	int high_x = qMax(ax2, bx2) + 20;
	int low_y = qMin(ay1, by1) - 20;
	int high_y = qMax(ay2, by2) + 20;

	setRect(QRectF(low_x, low_y, high_x - low_x, high_y - low_y));
	m_oGood.clear();
	m_oGood.append(QPoint(ax1, ay1));
	m_oGood.append(QPoint(bx1, by1));

	//set_rectangles(ax1, ax2, ay1, ay2, ap, bx1, bx2, by1, by2, bp);

	/*
	set_rectangles(
		(int) l_oR1.x(), (int) (l_oR1.x()+l_oR1.width()),
		(int) l_oR1.y(), (int) (l_oR1.y()+l_oR1.height()),
		m_iParent,
		(int) l_oR2.x(), (int) (l_oR2.x()+l_oR2.width()),
		(int) l_oR2.y(), (int) (l_oR2.y()+l_oR2.height()),
		m_iChild);

	int ret = num_seg();
	while (m_oLst.size() > ret)
		m_oLst.takeFirst();
	while (m_oLst.size() < ret)
		m_oLst<<QPoint();

	//qDebug()<<"begin dump";
	for (int i=0; i<ret; ++i)
	{
		int xx = get_vertex_x(i);
		int yy = get_vertex_y(i);
		m_oLst[i].setX(xx);
		m_oLst[i].setY(yy);
		//qDebug()<<xx<<yy;
	}
	//qDebug()<<"end dump";

	update_ratio();


	//bool l_b = false;
	//QList<QGraphicsItem *> l_oLst = m_oView->scene()->items(m_oView->m_oLastMovePoint);
	//foreach (QGraphicsItem *l_oItem, l_oLst)
	//{
	//	if (l_oItem->type() == BOX_ITEM_T)
	//	{
	//		box_item *l_oBox = (box_item*) l_oItem;
			//m_oView->add_select(l_oBox);
	//		l_b = true;
	//		break;
	//	}
	//}
	//if (not l_b)
	//{
	//	m_oView->deselect_all();
	//}
	*/
}


void box_link::update_ratio()
{
	/*
	// here we reset the offsets if necessary
	int ret = m_oLst.size();

	if (m_oOffsets.size() != ret - 2)
	{
		m_oOffsets.clear();
		for (int i=1; i<ret-1; ++i)
		{
			m_oOffsets.append(QPoint(0, 0));
		}
	}

	if (m_oGood.size() != ret)
	{
		m_oGood.clear();
		for (int i=0; i<ret; ++i) m_oGood.append(m_oLst[i]);
	}
	else
	{
		//just copy the data
		for (int i=0; i<ret; ++i) m_oGood[i] = m_oLst[i];
	}


	for (int i=0; i<m_oOffsets.size(); ++i)
	{
		int tmp = 0;
		if (m_oLst[i+1].x() == m_oLst[i+2].x())
		{
			tmp = m_oOffsets[i].x() + m_oLst[i+1].x();
			m_oGood[i+1].setX(int_val2(tmp));
			tmp = m_oOffsets[i].x() + m_oLst[i+2].x();
			m_oGood[i+2].setX(int_val2(tmp));
		}
		else
		{
			tmp = m_oOffsets[i].y() + m_oLst[i+1].y();
			m_oGood[i+1].setY(int_val2(tmp));
			tmp = m_oOffsets[i].y() + m_oLst[i+2].y();
			m_oGood[i+2].setY(int_val2(tmp));
		}
	}

	int mx1, mx2, my1, my2;
	mx1 = mx2 = m_oGood[0].x();
	my1 = my2 = m_oGood[0].y();

	for (int i=1; i<ret; ++i)
	{
		int nx = m_oGood[i].x(), ny = m_oGood[i].y();
		mx1 = qMin(mx1, nx);
		mx2 = qMax(mx2, nx);
		my1 = qMin(my1, ny);
		my2 = qMax(my2, ny);
	}

	// add a padding for the borders
	mx1 -= min_offset;
	mx2 += min_offset;
	my1 -= min_offset;
	my2 += min_offset;

	// now we have the size
	setRect(QRectF(mx1, my1, qAbs(mx2 - mx1), qAbs(my2 - my1)));
	*/
}

bool box_link::contains(const QPointF& i_oP) const
{
	// if the list of segments is not empty
	if (m_oGood.size() >= 1)
	{
		for (int i=0; i<m_oGood.size() - 1; ++i)
		{
			int x1 = qMin(m_oGood[i].x(), m_oGood[i+1].x());
			int x2 = qMax(m_oGood[i].x(), m_oGood[i+1].x());
			int y1 = qMin(m_oGood[i].y(), m_oGood[i+1].y());
			int y2 = qMax(m_oGood[i].y(), m_oGood[i+1].y());

			// vertical
			if (m_oGood[i].x() == m_oGood[i+1].x())
			{
				if (i_oP.x() >= x1 - 5 and i_oP.x() <= x2 + 5)
					if (i_oP.y() >= y1 and i_oP.y() <= y2)
						return true;
			}
			else //if (m_oGood[i].y() == m_oGood[i+1].y())
			{
				if (i_oP.y() >= y1 - 5 and i_oP.y() <= y2 + 5)
					if (i_oP.x() >= x1 and i_oP.x() <= x2)
						return true;
			}
		}
		return false;
	}
	return QGraphicsRectItem::contains(i_oP);
}

