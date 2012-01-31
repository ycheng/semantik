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

#define PAD 2
#define GRID 10

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
		m_oTop = new box_resize_point(m_oView);
		m_oTop->setRect(0, -CTRLSIZE, CTRLSIZE, CTRLSIZE);
		m_oTop->hide();
		m_oDown = new box_resize_point(m_oView);
		m_oDown->setRect(0, 0, CTRLSIZE, CTRLSIZE);
		m_oDown->hide();
		m_oLeft = m_oRight = NULL;
	}
	else
	{
		m_oLeft = new box_resize_point(m_oView);
		m_oLeft->setRect(0, 0, CTRLSIZE, CTRLSIZE);
		m_oLeft->hide();
		m_oRight = new box_resize_point(m_oView);
		m_oRight->setRect(-CTRLSIZE, 0, CTRLSIZE, CTRLSIZE);
		m_oRight->hide();
		m_oTop = m_oDown = NULL;
	}
	setRect(QRectF(QPointF(0, 0), size));
}

box_fork::~box_fork()
{

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
}

void box_fork::update_data()
{
	int px = m_oBox->m_iXX;
	int py = m_oBox->m_iYY;
	int ww = m_oBox->m_iWW;
	int hh = m_oBox->m_iHH;

	setPos(QPointF(m_oBox->m_iXX, m_oBox->m_iYY));
	Q_ASSERT(ww < 9999 && ww > 0);
	Q_ASSERT(hh < 9999 && hh > 0);
	QRectF r = QRectF(0, 0, ww, hh);
	setRect(r);

	if (m_oBox->m_bIsVertical)
	{
		m_oTop ->force_position(QPoint(px + ww/2., py));
		m_oDown->force_position(QPoint(px + ww/2., py + r.height()));
	}
	else
	{
		m_oLeft ->force_position(QPoint(px, py + hh / 2.));
		m_oRight->force_position(QPoint(px + ww, py + hh / 2.));
	}
}

QVariant box_fork::itemChange(GraphicsItemChange i_oChange, const QVariant &i_oValue)
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
			update_links();
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

