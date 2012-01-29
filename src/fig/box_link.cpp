// Thomas Nagy 2007-2012 GPLV3

#include <QApplication>
#include <QPainter>
#include <QtDebug>
#include "CON.h"
#include "box_link.h"
#include "box_view.h"
#include "data_item.h"
#include <box_control_point.h>

#define pad 25
#define MAX 2000000000
#define DAMP 1000000

box_link::box_link(box_view* i_oParent) : QGraphicsRectItem()
{
	m_oInnerLink.m_iParentPos = 0;
	m_oInnerLink.m_iChildPos = 0;

	m_oView = i_oParent;

	m_oLink = NULL;

	i_oParent->scene()->addItem(this);
	setZValue(98);

	setFlags(ItemIsSelectable);
	m_bReentrantLock = false;

	m_oStartPoint = new box_control_point(m_oView);
	m_oStartPoint->hide();
	m_oStartPoint->m_bIsSegment = false;
	m_oStartPoint->m_oLink = this;

	m_oEndPoint = new box_control_point(m_oView);
	m_oEndPoint->hide();
	m_oEndPoint->m_bIsSegment = false;
	m_oEndPoint->m_oLink = this;
	m_oEndPoint->setZValue(117);

	for (int i=0; i < 7; ++i)
	{
		box_control_point* b = new box_control_point(m_oView);
		b->hide();
		b->m_oLink = this;
		b->m_iOffset = i;
		m_oControlPoints.append(b);
	}
}

box_link::~box_link()
{
	foreach (box_control_point *b, m_oControlPoints) {
		delete b;
	}
	delete m_oStartPoint;
	delete m_oEndPoint;
}

void box_link::paint(QPainter *i_oPainter, const QStyleOptionGraphicsItem *option, QWidget * i_oW)
{
	//i_oPainter->setPen(pen());
	//QGraphicsRectItem::paint(i_oPainter, option, i_oW);

	QColor c = QColor(Qt::black);
	if (m_oLink)
		c = m_oLink->color;
	//if (isSelected())
	//	c = QColor(Qt::red);

	i_oPainter->setBrush(c);
        QPen l_oPen;
	if (m_oLink)
	{
		l_oPen.setWidth(m_oLink->border_width);
		l_oPen.setStyle(m_oLink->pen_style);
	}
	else
	{
		l_oPen.setWidth(1);
		l_oPen.setStyle(Qt::SolidLine);
	}

	l_oPen.setColor(c);
	i_oPainter->setPen(l_oPen);

	for (int i=0; i<m_oGood.size() - 1; ++i)
	{
		QLineF l_oLine(m_oGood[i].x(), m_oGood[i].y(), m_oGood[i+1].x(), m_oGood[i+1].y());
		i_oPainter->drawLine(l_oLine);
		//qDebug()<<m_oGood[i].x()<<m_oGood[i].y()<<m_oGood[i+1].x()<<m_oGood[i+1].y();
	}
	//QPen l_oPen = pen();
	//l_oPen.setStyle(Qt::SolidLine);
	//i_oPainter->setPen(l_oPen);

      	l_oPen.setStyle(Qt::SolidLine);

	//draw_triangle(i_oPainter, m_iParent, m_oGood[0]);
	draw_triangle(i_oPainter, m_oInnerLink.m_iChildPos, m_oGood[m_oGood.size()-1]);
}


#define xw 3.
#define yw 8.

void box_link::draw_triangle(QPainter *i_oPainter, int i_iPos, QPointF i_oP)
{
	QPolygonF l_oPol;
	l_oPol<<i_oP;
	switch (i_iPos)
	{
		case data_link::NORTH:
			{
				l_oPol<<i_oP+QPointF(-xw, -yw);
				l_oPol<<i_oP+QPointF( xw, -yw);
			}
			break;
		case data_link::WEST:
			{
				l_oPol<<i_oP+QPointF(-yw, -xw);
				l_oPol<<i_oP+QPointF(-yw,  xw);
			}
			break;
		case data_link::SOUTH:
			{
                                l_oPol<<i_oP+QPointF(-xw, yw);
                                l_oPol<<i_oP+QPointF( xw, yw);
                        }
			break;
		case data_link::EAST:
			{
                                l_oPol<<i_oP+QPointF(yw, -xw);
                                l_oPol<<i_oP+QPointF(yw,  xw);
                        }
			break;
		default:
			Q_ASSERT(false);
	}
	i_oPainter->drawPolygon(l_oPol);
}

void box_link::set_rectangles(int ax1, int ax2, int ay1, int ay2, int ap, int bx1, int bx2, int by1, int by2, int bp)
{
	hor.clear();
	ver.clear();
	graph.clear();
	prev.clear();
	dist.clear();
	m_oLst.clear();

	ap = ap & data_link::COORD;
	bp = bp & data_link::COORD;

	switch (ap) {
		case data_link::NORTH:
			hor.append((ax1 + ax2) / 2);
			ver.append(ay1);
			break;
		case data_link::WEST:
			hor.append(ax1);
			ver.append((ay1 + ay2) / 2);
			break;
		case data_link::SOUTH:
			ver.append(ay2);
			hor.append((ax1 + ax2) / 2);
			break;
		case data_link::EAST:
			hor.append(ax2);
			ver.append((ay1 + ay2) / 2);
			break;
		default:
			Q_ASSERT(false);
	}
	QPair<int,int> init_p(hor.at(0), ver.at(0));

	switch (bp) {
		case data_link::NORTH:
			hor.append((bx1 + bx2) / 2);
			ver.append(by1);
			break;
		case data_link::WEST:
			hor.append(bx1);
			ver.append((by1 + by2) / 2);
			break;
		case data_link::SOUTH:
			ver.append(by2);
			hor.append((bx1 + bx2) / 2);
			break;
		case data_link::EAST:
			hor.append(bx2);
			ver.append((by1 + by2) / 2);
			break;
		default:
			Q_ASSERT(false);
	}
	QPair<int, int> end_p(hor.at(1), ver.at(1));

	if (ap == data_link::WEST && bp == data_link::EAST && bx2 >= ax1 - pad)
	{
		hor.append(ax1 - pad);
		hor.append(bx2 + pad);
	}
	else if (ap == data_link::EAST && bp == data_link::WEST && ax2 >= bx1 - pad)
	{
		hor.append(bx1 - pad);
		hor.append(ax2 + pad);
	}
	else if (ap == data_link::NORTH && bp == data_link::SOUTH && by2 >= ay1 - pad)
	{
		ver.append(ay1 - pad);
		ver.append(by2 + pad);
	}
	else if (ap == data_link::SOUTH && bp == data_link::NORTH && ay2 >= by1 - pad)
	{
		ver.append(by1 - pad);
		ver.append(ay2 + pad);
	}

	hor.append(qMin(ax1, bx1) - pad);
	hor.append(qMax(ax2, bx2) + pad);
	ver.append(qMin(ay1, by1) - pad);
	ver.append(qMax(ay2, by2) + pad);

	if (ax1 > bx2) hor.append((ax1 + bx2) / 2);
	if (bx1 > ax2) hor.append((bx1 + ax2) / 2);
	if (ay1 > by2) ver.append((ay1 + by2) / 2);
	if (by1 > ay2) ver.append((by1 + ay2) / 2);

	foreach (int x, hor) {
		foreach (int y, ver) {
			QPair<int, int> p(x, y);

			graph << p;
			dist[p] = MAX;
			prev[p] = init_p;
		}
	}
	dist[init_p] = 0;

	while (graph.size())
	{

		int mi = MAX;
		QPair<int, int> cand;
		QPair<int, int> p;
		bool ok = false;
		foreach (p, graph) {
			if (dist[p] < mi) {
				mi = dist[p];
				cand = p;
				ok = true;
			}
		}

		if (!ok) {
			m_oLst.clear();
			m_oLst.append(QPoint(init_p.first, init_p.second));
			m_oLst.append(QPoint(end_p.first, end_p.second));
			break;
		}

		graph.remove(cand);
		if (cand == end_p) {
			QPair<int, int> cand = end_p;
			while (cand != init_p) {
				m_oLst.prepend(QPoint(cand.first, cand.second));
				cand = prev[cand];
			}
			m_oLst.prepend(QPoint(cand.first, cand.second));
			break;
		}

		foreach (p, graph) {
			if (p.first == cand.first || p.second == cand.second)
			{
				if (may_use(cand, p, ax1, ax2, ay1, ay2, bx1, bx2, by1, by2))
				{
					int newdist = dist[cand] + qAbs(p.first - cand.first) + qAbs(p.second - cand.second) + DAMP;
					if (newdist < dist[p])
					{
						dist[p] = newdist;
						prev[p] = cand;
					}
				}
			}
		}
	}
}

int box_link::may_use(QPair<int, int> cand, QPair<int, int> p, int ax1, int ax2, int ay1, int ay2, int bx1, int bx2, int by1, int by2)
{
	int cx1 = qMin(cand.first, p.first);
	int cx2 = qMax(cand.first, p.first);
	int cy1 = qMin(cand.second, p.second);
	int cy2 = qMax(cand.second, p.second);

	return 1 && (
		cx1 >= ax2 && cx2 > ax2 ||
		cx2 <= ax1 && cx1 < ax1 ||
		cy1 >= ay2 && cy2 > ay2 ||
		cy2 <= ay1 && cy1 < ay1
	) && (
		cx1 >= bx2 && cx2 > bx2 ||
		cx2 <= bx1 && cx1 < bx1 ||
		cy1 >= by2 && cy2 > by2 ||
		cy2 <= by1 && cy1 < by1
	);
}


void box_link::update_pos()
{
	QRectF l_oR1, l_oR2;

	if (connectable *start = m_oView->m_oItems.value(m_oInnerLink.m_iParent))
	{
		m_oInnerLink.m_oStartPoint = start->get_point(m_oInnerLink.m_iParentPos);
		l_oR1 = start->rect();
	}
	else
	{
		QPointF l_oP = m_oInnerLink.m_oStartPoint;
		l_oR1 = QRectF(l_oP - QPointF(1, 1), l_oP + QPointF(1, 1));
	}
	m_oStartPoint->force_position(m_oInnerLink.m_oStartPoint);

	if (connectable *end = m_oView->m_oItems.value(m_oInnerLink.m_iChild))
	{
		m_oInnerLink.m_oEndPoint = end->get_point(m_oInnerLink.m_iChildPos);
		l_oR2 = end->rect();
	}
	else
	{
		QPointF l_oP = m_oInnerLink.m_oEndPoint;
		l_oR2 = QRectF(l_oP - QPointF(1, 1), l_oP + QPointF(1, 1));
	}
	m_oEndPoint->force_position(m_oInnerLink.m_oEndPoint);

	int ax1 = (int) l_oR1.x();
	int ax2 = (int) (l_oR1.x()+l_oR1.width());
	int ay1 = (int) l_oR1.y();
	int ay2 = (int) (l_oR1.y()+l_oR1.height());

	int bx1 = (int) l_oR2.x();
	int bx2 = (int) (l_oR2.x()+l_oR2.width());
	int by1 = (int) l_oR2.y();
	int by2 = (int) (l_oR2.y()+l_oR2.height());

	set_rectangles(ax1, ax2, ay1, ay2, m_oInnerLink.m_iParentPos, bx1, bx2, by1, by2, m_oInnerLink.m_iChildPos);

	/*
	//qDebug()<<"begin dump";
	for (int i=0; i<ret; ++i)
	{
		int xx = get_vertex_x(i);
		int yy = get_vertex_y(i);
		m_oLst[i].setX(xx);
		m_oLst[i].setY(yy);
		//qDebug()<<xx<<yy;
	}*/
	//qDebug()<<"end dump";

	update_ratio();
	update();
}

void box_link::update_offset(const QPointF& i_oP, int i_iIdx)
{
	if (m_bReentrantLock) return;
	m_bReentrantLock = true;

	update_ratio();
	for (int i = 0; i < m_oLst.size() - 3; ++i)
	{
		box_control_point * b = m_oControlPoints.at(i);
		if (i != i_iIdx) {
			b->init_pos();
		}
	}
	m_bReentrantLock = false;
	update();
}

void box_link::update_ratio()
{
	// here we reset the offsets if necessary
	int ret = m_oLst.size();

	if (m_oGood.size() != ret)
	{
		m_oGood.clear();
		for (int i=0; i<ret; ++i) m_oGood.append(m_oLst[i]);
	}
	else
	{
		for (int i=0; i<ret; ++i) m_oGood[i] = m_oLst[i];
	}

	if (m_oLink)
	{
		if (m_oLink->m_oOffsets.size() != ret - 3)
		{
			m_oLink->m_oOffsets.clear();
			for (int i=1; i < ret - 2; ++i)
			{
				m_oLink->m_oOffsets.append(QPoint(0, 0));
			}
		}

		for (int i=0; i < m_oLink->m_oOffsets.size(); ++i)
		{
			if (m_oLst[i+1].x() == m_oLst[i+2].x())
			{
				int v = m_oLink->m_oOffsets[i].x() + m_oLst[i+1].x();
				v = int_val2(v);
				m_oGood[i+1].setX(v);
				m_oGood[i+2].setX(v);
			}
			else if (m_oLst[i+1].y() == m_oLst[i+2].y())
			{
				int v = m_oLink->m_oOffsets[i].y() + m_oLst[i+1].y();
				v = int_val2(v);
				m_oGood[i+1].setY(v);
				m_oGood[i+2].setY(v);
			}
		}
	}

	int mx1, mx2, my1, my2;
	mx1 = mx2 = m_oGood[0].x();
	my1 = my2 = m_oGood[0].y();

	for (int i = 1; i < ret; ++i)
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

	for (int i = 0; i < m_oControlPoints.size(); ++i)
	{
		box_control_point * b = m_oControlPoints.at(i);
		if (i < m_oGood.size() - 3)
		{
			b->init_pos();

			if (isSelected())
				b->show();
			else
				b->hide();
		}
		else
		{
			b->hide();
		}
	}

	QPainterPath p;
	for (int i = 0; i < m_oGood.size() - 1; ++i)
	{
		int x1 = qMin(m_oGood[i].x(), m_oGood[i+1].x());
		int x2 = qMax(m_oGood[i].x(), m_oGood[i+1].x());
		int y1 = qMin(m_oGood[i].y(), m_oGood[i+1].y());
		int y2 = qMax(m_oGood[i].y(), m_oGood[i+1].y());

		if (x1 == x2)
		{
			p.addRect(x1 - 5, y1, 10, y2 - y1);
		}
		else
		{
			p.addRect(x1, y1 - 5, x2 - x1, 10);
		}
	}

	inner_shape = p;
}

QPainterPath box_link::shape() const
{
	return inner_shape;
}

QVariant box_link::itemChange(GraphicsItemChange i_oChange, const QVariant &i_oValue)
{
	QVariant l_oRet = QGraphicsItem::itemChange(i_oChange, i_oValue);
	if (scene())
	{
		if (i_oChange == ItemSelectedHasChanged)
		{
			if (isSelected())
			{
				setZValue(102);
				m_oStartPoint->show();
				m_oEndPoint->show();
			}
			else
			{
				setZValue(98);
				m_oStartPoint->hide();
				m_oEndPoint->hide();
			}
			update_pos();
		}
	}

	return QGraphicsItem::itemChange(i_oChange, i_oValue);
}

