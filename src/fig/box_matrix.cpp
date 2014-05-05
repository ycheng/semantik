// Thomas Nagy 2007-2014 GPLV3

#include <QApplication>
#include <QAbstractTextDocumentLayout>
#include <QTextDocument>
#include <QTextDocumentFragment>
#include <QTextList>
#include <QClipboard>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QtDebug>
#include <QAction>
#include <QTextDocument>
#include "box_matrix.h"
#include "data_item.h"
 #include "res:zable.h"
#include "box_view.h"
 #include "box_link.h"
#include "sem_mediator.h"
#include "mem_box.h"

#include <QFont>

#define PAD 2
#define MIN_FORK_SIZE 30

box_matrix::box_matrix(box_view* view, int id) : box_item(view, id)
{
	QFont font = doc.defaultFont();
	font.setPointSize(font.pointSize() - 2);
	doc.setDefaultFont(font);

	setZValue(80);
}

void box_matrix::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	painter->save();

	QRectF l_oRect = boundingRect().adjusted(PAD, PAD, -PAD, -PAD);

	QPen l_oPen = QPen(Qt::SolidLine);
	l_oPen.setColor(Qt::black);
	l_oPen.setCosmetic(false);
	l_oPen.setWidth(1);
	painter->setPen(l_oPen);


	qreal l_iHref = l_oRect.top();
	foreach (int l_iHoff, m_oBox->m_oRowSizes)
	{
		l_iHref += l_iHoff;
		painter->drawLine(l_oRect.left(), l_iHref, l_oRect.right(), l_iHref);
	}
	l_iHref = l_oRect.left();
	foreach (int l_iHoff, m_oBox->m_oColSizes)
	{
		l_iHref += l_iHoff;
		painter->drawLine(l_iHref, l_oRect.top(), l_iHref, l_oRect.bottom());
	}

	painter->drawRect(l_oRect);

	if (isSelected())
	{
		QRectF l_oR(-8, -8, 6, 6);
		painter->setBrush(QColor("#FFFF00"));

		qreal l_i = 6 - PAD;
		foreach (int l_iSize, m_oBox->m_oRowSizes) {
			l_i += l_iSize;
			painter->drawRect(l_oR.translated(m_iWW, l_i));
		}

		l_i = 6 - PAD;
		foreach (int l_iSize, m_oBox->m_oColSizes) {
			l_i += l_iSize;
			painter->drawRect(l_oR.translated(l_i, m_iHH));
		}

		painter->drawRect(l_oR.translated(m_iWW, m_iHH));
	}

	painter->restore();
}

void box_matrix::mouseMoveEvent(QGraphicsSceneMouseEvent *e)
{
	if (m_bMoving)
	{
		QPointF np = e->pos();
		int x = np.x() - m_oLastPressPoint.x();
		int y = np.y() - m_oLastPressPoint.y();

		m_iWW = m_oBox->m_iWW + x;
		if (m_iWW < 2 * GRID) m_iWW = 2 * GRID;
		m_iWW = grid_int(m_iWW);

		int l_i = 0;
		foreach (int l_iSize, m_oBox->m_oColSizes) {
			l_i += l_iSize;
		}
		if (m_iWW < l_i + 2 * GRID) {
			m_iWW = l_i + 2 * GRID;
		}

		m_iHH = m_oBox->m_iHH + y;
		if (m_iHH < 2 * GRID) m_iHH = 2 * GRID;
		m_iHH = grid_int(m_iHH);

		l_i = 0;
		foreach (int l_iSize, m_oBox->m_oRowSizes) {
			l_i += l_iSize;
		}
		if (m_iHH < l_i + 2 * GRID) {
			m_iHH = l_i + 2 * GRID;
		}

		doc.setTextWidth(m_iWW - 2 * OFF);
		setRect(0, 0, m_iWW, m_iHH);
		m_oChain->setPos(m_iWW + 3, 0);

		m_oView->message(m_oView->trUtf8("%1 x %2").arg(QString::number(m_iWW), QString::number(m_iHH)), 1000);

		update();
		update_links();
	}
	else
	{
		QGraphicsRectItem::mouseMoveEvent(e);
	}
}

/*
void box_matrix::freeze(bool b)
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

QVariant box_matrix::itemChange(GraphicsItemChange i_oChange, const QVariant &i_oValue)
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
			update_sizers();
		}
		else if (i_oChange == ItemSelectedHasChanged)
		{
			bool b = isSelected();
			foreach(box_resize_point *l_o, m_oColPoints) {
				l_o->setVisible(b);
			}
			foreach(box_resize_point *l_o, m_oRowPoints) {
				l_o->setVisible(b);
			}
		}
	}

	return QGraphicsItem::itemChange(i_oChange, i_oValue);
}

void box_matrix::update_sizers()
{
	QPointF p = pos();
	while (m_oRowPoints.size() > m_oBox->m_oRowSizes.size()) {
                delete m_oRowPoints.takeFirst();
        }
	while (m_oRowPoints.size() < m_oBox->m_oRowSizes.size()) {
		box_resize_point *l_o = new box_resize_point(m_oView, this);
		l_o->hide();
		l_o->setRect(-CTRLSIZE/2., 0, CTRLSIZE, CTRLSIZE);
		m_oRowPoints.append(l_o);
	}

	while (m_oColPoints.size() > m_oBox->m_oColSizes.size()) {
                delete m_oColPoints.takeFirst();
        }
	while (m_oColPoints.size() < m_oBox->m_oColSizes.size()) {
		box_resize_point *l_o = new box_resize_point(m_oView, this);
		l_o->hide();
		l_o->setRect(-CTRLSIZE/2., 0, CTRLSIZE, CTRLSIZE);
		m_oColPoints.append(l_o);
	}

	QRectF l_oRect = rect();
	int l_iYpos = p.y() + l_oRect.height() - CTRLSIZE + PAD/2.;
	int l_iOff = p.x() - PAD/2.;
	int i = 0;
	foreach (int l_iSize, m_oBox->m_oColSizes) {
		l_iOff += l_iSize;
		m_oColPoints.at(i)->setPos(l_iOff, l_iYpos);
		i++;
	}

	int l_iXpos = p.x() + l_oRect.width() - CTRLSIZE / 2. + PAD / 2.;
	l_iOff = p.y() - CTRLSIZE/2. - PAD / 2.;
	i = 0;
	foreach (int l_iSize, m_oBox->m_oRowSizes) {
		l_iOff += l_iSize;
		m_oRowPoints.at(i)->setPos(l_iXpos, l_iOff);
		i++;
	}
}
*/

