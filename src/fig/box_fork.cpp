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
#include "box_fork.h"
#include "data_item.h"
 #include "res:zable.h"
#include "box_view.h"
 #include "box_link.h"
#include "sem_mediator.h"
#include "mem_box.h"


#define PAD 2
#define MIN_FORK_SIZE 30

box_fork::box_fork(box_view* i_oParent, int i_iId) : QGraphicsRectItem(), connectable(), resizable(), m_oView(i_oParent)
{
	m_iId = i_iId;
	m_oItem = m_oView->m_oMediator->m_oItems[m_oView->m_iId];
	m_oBox = m_oItem->m_oBoxes[m_iId];
	Q_ASSERT(m_oBox);

	i_oParent->scene()->addItem(this);

	setCacheMode(QGraphicsItem::DeviceCoordinateCache);

	setZValue(100);
	setFlags(ItemIsMovable | ItemIsSelectable | ItemSendsGeometryChanges);

	QSizeF size(FORK_LENGTH, FORK_WIDTH);
	if (m_oBox->m_bIsVertical)
	{
		size.transpose();
		m_oTop = new box_resize_point(m_oView, this);
		m_oTop->setRect(-CTRLSIZE/2., 0, CTRLSIZE, CTRLSIZE);
		m_oTop->setCursor(Qt::SizeVerCursor); // FIXME if someone has a solution for this
		m_oTop->hide();
		m_oDown = new box_resize_point(m_oView, this);
		m_oDown->setRect(-CTRLSIZE/2., -CTRLSIZE, CTRLSIZE, CTRLSIZE);
		m_oDown->setCursor(Qt::SizeVerCursor);
		m_oDown->hide();
		m_oLeft = m_oRight = NULL;
	}
	else
	{
		m_oLeft = new box_resize_point(m_oView, this);
		m_oLeft->setRect(0, -CTRLSIZE/2., CTRLSIZE, CTRLSIZE);
		m_oLeft->setCursor(Qt::SizeHorCursor);
		m_oLeft->hide();
		m_oRight = new box_resize_point(m_oView, this);
		m_oRight->setRect(-CTRLSIZE, -CTRLSIZE/2., CTRLSIZE, CTRLSIZE);
		m_oRight->setCursor(Qt::SizeHorCursor);
		m_oRight->hide();
		m_oTop = m_oDown = NULL;
	}
	setRect(QRectF(QPointF(0, 0), size));
}

box_fork::~box_fork()
{
	delete m_oTop;
	delete m_oDown;
	delete m_oLeft;
	delete m_oRight;
}

void box_fork::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	painter->save();

	QRectF l_oRect = boundingRect().adjusted(PAD, PAD, -PAD, -PAD);

	QPen l_oPen = QPen(Qt::SolidLine);
	l_oPen.setColor(Qt::black);
	if (isSelected()) l_oPen.setStyle(Qt::DotLine);
	l_oPen.setCosmetic(false);
	l_oPen.setWidth(1);
	painter->setBrush(m_oBox->color);
	painter->drawRect(l_oRect);

	painter->restore();
}

void box_fork::mousePressEvent(QGraphicsSceneMouseEvent* e) {
	setZValue(100);
	QGraphicsRectItem::mousePressEvent(e);
}

void box_fork::mouseReleaseEvent(QGraphicsSceneMouseEvent* e) {
	setZValue(99);
	QGraphicsRectItem::mouseReleaseEvent(e);

	qDebug()<<"box fork release event?";

	int px = m_oBox->m_iXX;
	int py = m_oBox->m_iYY;
	int ww = m_oBox->m_iWW;
	int hh = m_oBox->m_iHH;
	if (m_oBox->m_bIsVertical)
	{
		m_oTop ->setPos(QPoint(px + ww/2., py));
		m_oDown->setPos(QPoint(px + ww/2., py + hh));
	}
	else
	{
		//m_oLeft ->force_position(QPoint(px, py + hh / 2.));
		//m_oRight->force_position(QPoint(px + ww, py + hh / 2.));
		m_oLeft ->setPos(QPoint(px, py + hh / 2.));
		m_oRight->setPos(QPoint(px + ww, py + hh / 2.));
	}

}

void box_fork::update_data()
{
	qDebug()<<"update data";


	int px = m_oBox->m_iXX;
	int py = m_oBox->m_iYY;
	int ww = m_oBox->m_iWW;
	int hh = m_oBox->m_iHH;


	setPos(QPointF(m_oBox->m_iXX, m_oBox->m_iYY));
	Q_ASSERT(ww < 9999 && ww > 0);
	Q_ASSERT(hh < 9999 && hh > 0);
	QRectF r = QRectF(0, 0, ww, hh);
	setRect(r);

	px = pos().x();
	py = pos().y();


	if (m_oBox->m_bIsVertical)
	{
		//m_oTop ->force_position(QPoint(px + ww/2., py));
		//m_oDown->force_position(QPoint(px + ww/2., py + r.height()));
		m_oTop ->setPos(QPoint(px + ww/2., py));
		m_oDown->setPos(QPoint(px + ww/2., py + hh));
	}
	else
	{
		//m_oLeft ->force_position(QPoint(px, py + hh / 2.));
		//m_oRight->force_position(QPoint(px + ww, py + hh / 2.));
		m_oLeft ->setPos(QPoint(px, py + hh / 2.));
		m_oRight->setPos(QPoint(px + ww, py + hh / 2.));
	}

}

QVariant box_fork::itemChange(GraphicsItemChange i_oChange, const QVariant &i_oValue)
{
	if (scene())
	{
		if (i_oChange == ItemPositionChange)
		{
			QPointF np = i_oValue.toPointF();

			/*if (m_oLeft && m_oLeft->m_bHeld)
			{
				//np.setX(m_oBox->m_iXX + m_oBox->m_iWW / 2.);
				np.setY(m_oBox->m_iYY + m_oBox->m_iHH / 2.);
				return np;
			}
			else if (m_oRight && m_oRight->m_bHeld)
			{
				//np.setX(m_oBox->m_iXX + m_oBox->m_iWW / 2.);
				np.setY(m_oBox->m_iYY + m_oBox->m_iHH / 2.);
				return np;

			}
			else*/
			{
				np.setX(((int) np.x() / GRID) * GRID);
				np.setY(((int) np.y() / GRID) * GRID);
				return np;
			}
		}
		else if (i_oChange == ItemPositionHasChanged)
		{
			//qDebug()<<"item position changed event on box_fork";
			//update_links();
		}
		else if (i_oChange == ItemSelectedHasChanged)
		{
			bool b = isSelected();
			if (b)
			{
				setZValue(101);
			}
			else
			{
				setZValue(100);
			}

			if (m_oLeft)  m_oLeft->setVisible(b);
			if (m_oRight) m_oRight->setVisible(b);
			if (m_oTop)   m_oTop->setVisible(b);
			if (m_oDown)  m_oDown->setVisible(b);
		}
	}

	return QGraphicsItem::itemChange(i_oChange, i_oValue);
}

void box_fork::update_links()
{
	// FIXME
	foreach (box_link* l_oLink, m_oView->m_oLinks)
	{
		l_oLink->update_pos();
	}
}

static int RATIO[] = {250, 333, 500, 667, 750, 0};
#define MUL 64

int box_fork::choose_position(const QPointF& i_oP, int id)
{
	QRectF r = rect();
	QPointF l_o = pos() - i_oP + QPointF(r.width()/2, r.height()/2);

	double c_x = l_o.x() * r.height();
	double c_y = l_o.y() * r.width();

	int ret = 0;
	int best = 1<<30;
	int cand = 0;
	if (m_oBox->m_bIsVertical)
	{
		ret = (c_x > 0) ? data_link::WEST : data_link::EAST;
		for (int i=0; i < 10; ++i) {
			int k = RATIO[i];
			if (k == 0) break;
			int val = qAbs((k * r.height() / 1000.) - (i_oP.y() - pos().y()));

			if (val < best)
			{
				best = val;
				cand = k;
			}
		}
		ret += cand * MUL;
	}
	else
	{
		ret = (c_y > 0) ? data_link::NORTH : data_link::SOUTH;
		for (int i=0; i < 10; ++i) {
			int k = RATIO[i];
			if (k == 0) break;

			int val = qAbs((k * r.width() / 1000.) - (i_oP.x() - pos().x()));
			if (val < best)
			{
				best = val;
				cand = k;
			}
		}
		ret += cand * MUL;
	}
	return ret;
}

QPoint box_fork::get_point(int i_oP)
{
	QRectF r = rect();
	int ratio = i_oP / MUL;

	if (ratio >= 1000 || ratio <= 0) ratio = 500;
	switch (i_oP & data_link::COORD) {
		case data_link::NORTH:
			return QPoint(r.x() + r.width() * ratio / 1000., r.y());
		case data_link::WEST:
			return QPoint(r.x(), r.y() + r.height() * ratio / 1000.);
		case data_link::SOUTH:
			return QPoint(r.x() + r.width() * ratio / 1000., r.y() + r.height());
		case data_link::EAST:
			return QPoint(r.x() + r.width(), r.y() + r.height() * ratio / 1000.);
	}
	Q_ASSERT(false);
	return QPoint(0, 0);
}


QPointF box_fork::validate_point(box_resize_point *p, const QPointF & orig)
{
	QPointF pf = QPointF(orig);
	QSizeF s;
	if (p == m_oTop)
	{
		pf.setX(m_oBox->m_iXX + m_oBox->m_iWW / 2.);

		m_iLastStretch = - pf.y() + m_oBox->m_iYY + m_oBox->m_iHH;
		m_iLastStretch = GRID * (m_iLastStretch / GRID);
		if (m_iLastStretch < MIN_FORK_SIZE) m_iLastStretch = MIN_FORK_SIZE;
		pf.setY(m_oBox->m_iYY + m_oBox->m_iHH - m_iLastStretch);
		setPos(m_oBox->m_iXX, pf.y());

		s = QSizeF(FORK_WIDTH, m_iLastStretch);
	}
	else if (p == m_oDown)
	{
		pf.setX(m_oBox->m_iXX + m_oBox->m_iWW / 2.);

		m_iLastStretch = pf.y() - m_oBox->m_iYY;
		m_iLastStretch = GRID * (m_iLastStretch / GRID);
		if (m_iLastStretch < MIN_FORK_SIZE) m_iLastStretch = MIN_FORK_SIZE;
		pf.setY(m_oBox->m_iYY + m_iLastStretch);

		s = QSizeF(FORK_WIDTH, m_iLastStretch);
	}
	else if (p == m_oLeft)
	{
		pf.setY(m_oBox->m_iYY + m_oBox->m_iHH / 2.);

		m_iLastStretch = - pf.x() + m_oBox->m_iXX + m_oBox->m_iWW;
		m_iLastStretch = GRID * (m_iLastStretch / GRID);
		if (m_iLastStretch < MIN_FORK_SIZE) m_iLastStretch = MIN_FORK_SIZE;
		pf.setX(m_oBox->m_iXX + m_oBox->m_iWW - m_iLastStretch);
		setPos(pf.x(), m_oBox->m_iYY);

		s = QSizeF(m_iLastStretch, FORK_WIDTH);
	}
	else if (p == m_oRight)
	{
		pf.setY(m_oBox->m_iYY + m_oBox->m_iHH / 2.);
		m_iLastStretch = pf.x() - m_oBox->m_iXX;
		m_iLastStretch = GRID * (m_iLastStretch / GRID);
		if (m_iLastStretch < MIN_FORK_SIZE) m_iLastStretch = MIN_FORK_SIZE;
		pf.setX(m_oBox->m_iXX + m_iLastStretch);

		s = QSizeF(m_iLastStretch, FORK_WIDTH);
	}
	setRect(QRectF(QPointF(0, 0), s));
	return pf;
}

void box_fork::commit_size(box_resize_point *p)
{
	QRect r_orig(m_oBox->m_iXX, m_oBox->m_iYY, m_oBox->m_iWW, m_oBox->m_iHH);
	QRect r_dest;

	if (p == m_oTop)
	{
		r_dest.setX(m_oBox->m_iXX);
		r_dest.setY(m_oBox->m_iYY + m_oBox->m_iHH - m_iLastStretch);
		r_dest.setWidth(m_oBox->m_iWW);
		r_dest.setHeight(m_iLastStretch);
	}
	else if (p == m_oDown)
	{
		r_dest.setX(m_oBox->m_iXX);
		r_dest.setY(m_oBox->m_iYY);
		r_dest.setWidth(m_oBox->m_iWW);
		r_dest.setHeight(m_iLastStretch);
	}
	else if (p == m_oLeft)
	{
		r_dest.setX(m_oBox->m_iXX + m_oBox->m_iWW - m_iLastStretch);
		r_dest.setY(m_oBox->m_iYY);
		r_dest.setHeight(m_oBox->m_iHH);
		r_dest.setWidth(m_iLastStretch);
	}
	else if (p == m_oRight)
	{
		r_dest.setX(m_oBox->m_iXX);
		r_dest.setY(m_oBox->m_iYY);
		r_dest.setHeight(m_oBox->m_iHH);
		r_dest.setWidth(m_iLastStretch);
	}

	mem_size_box *mem = new mem_size_box(m_oView->m_oMediator, m_oView->m_iId);
	mem->prev_values[m_oBox] = r_orig;
	mem->next_values[m_oBox] = r_dest;
	mem->apply();
}

void box_fork::freeze(bool b)
{
	if (b)
	{
		setFlags(ItemIsSelectable);
		m_iLastStretch = 0;
	}
	else
	{
		setFlags(ItemIsMovable | ItemIsSelectable | ItemSendsGeometryChanges);
	}
}

