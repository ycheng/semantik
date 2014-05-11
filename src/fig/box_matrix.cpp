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

	QRectF l_oR2 = boundingRect();
	qreal l_oX = l_oR2.right();
	qreal l_oY = l_oR2.bottom();

	if (isSelected())
	{
		QRectF l_oR(-8, -8, 6, 6);
		painter->setBrush(QColor("#FFFF00"));

		qreal l_i = 6 - PAD;
		foreach (int l_iSize, m_oBox->m_oRowSizes) {
			l_i += l_iSize;
			painter->drawRect(l_oR.translated(l_oX, l_i));
		}

		l_i = 6 - PAD;
		foreach (int l_iSize, m_oBox->m_oColSizes) {
			l_i += l_iSize;
			painter->drawRect(l_oR.translated(l_i, l_oY));
		}

		painter->drawRect(l_oR.translated(l_oX, l_oY));
	}

	painter->restore();
}

void box_matrix::mousePressEvent(QGraphicsSceneMouseEvent* e)
{
	m_oLastPressPoint = e->pos();
	QRectF l_oR(-10, -10, 8, 8);

	qreal l_i = 6 - PAD;
	int i = 0;
	foreach (int l_iSize, m_oBox->m_oRowSizes) {
		l_i += l_iSize;
		if (l_oR.translated(m_iWW, l_i).contains(m_oLastPressPoint))
		{
			m_iLastSize = l_iSize;
			m_iMovingRow = i;
			m_iMovingCol = -1;
			setFlags(ItemIsSelectable | ItemSendsGeometryChanges);
			m_bMoving = true;
			QGraphicsRectItem::mousePressEvent(e);
			return;
		}
		i += 1;
	}

	i = 0;
	l_i = 6 - PAD;
	foreach (int l_iSize, m_oBox->m_oColSizes) {
		l_i += l_iSize;
		if (l_oR.translated(l_i, m_iHH).contains(m_oLastPressPoint))
		{
			m_iLastSize = l_iSize;
			m_iMovingRow = -1;
			m_iMovingCol = i;
			setFlags(ItemIsSelectable | ItemSendsGeometryChanges);
			m_bMoving = true;
			QGraphicsRectItem::mousePressEvent(e);
			return;
		}
		i += 1;
	}

	if (l_oR.translated(m_iWW, m_iHH).contains(m_oLastPressPoint))
	{
		m_iMovingRow = m_iMovingCol = -1;
		setFlags(ItemIsSelectable | ItemSendsGeometryChanges);
		m_bMoving = true;
		QGraphicsRectItem::mousePressEvent(e);
		return;
	}
	QGraphicsRectItem::mousePressEvent(e);
}

void box_matrix::mouseMoveEvent(QGraphicsSceneMouseEvent *e)
{
	if (m_bMoving)
	{
		QPointF np = e->pos();
		int x = np.x() - m_oLastPressPoint.x();
		int y = np.y() - m_oLastPressPoint.y();

		if (m_iMovingRow == -1 && m_iMovingCol == -1) {
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

			doc.setTextWidth(m_iWW - 2 * OFF); // TODO: doc?
			setRect(0, 0, m_iWW, m_iHH);
			m_oChain->setPos(m_iWW + 3, 0);

			m_oView->message(m_oView->trUtf8("%1 x %2").arg(QString::number(m_iWW), QString::number(m_iHH)), 1000);
		}
		else if (m_iMovingRow > -1)
		{
			int l_iSize = grid_int(m_iLastSize + y);
			if (l_iSize < 2 * GRID) {
				l_iSize = 2 * GRID;
			}
			m_oBox->m_oRowSizes[m_iMovingRow] = l_iSize;
			setRect(0, 0, m_oBox->m_iWW, m_oBox->m_iHH + l_iSize - m_iLastSize);
		}
		else if (m_iMovingCol > -1)
		{
			int l_iSize = grid_int(m_iLastSize + x);
			if (l_iSize < 2 * GRID) {
				l_iSize = 2 * GRID;
			}
			m_oBox->m_oColSizes[m_iMovingCol] = l_iSize;
			setRect(0, 0, m_oBox->m_iWW + l_iSize - m_iLastSize, m_oBox->m_iHH);
		}

		update();
		update_links();
	}
	else
	{
		QGraphicsRectItem::mouseMoveEvent(e);
	}
}

void box_matrix::mouseReleaseEvent(QGraphicsSceneMouseEvent* e) {
	if (m_bMoving)
	{
		m_bMoving = false;
		setFlags(ItemIsMovable | ItemIsSelectable | ItemSendsGeometryChanges);

		if (m_iWW != m_oBox->m_iWW || m_iHH != m_oBox->m_iHH)
		{
			mem_size_box *mem = new mem_size_box(m_oView->m_oMediator, m_oView->m_iId);
			mem->prev_values[m_oBox] = QRect(m_oBox->m_iXX, m_oBox->m_iYY, m_oBox->m_iWW, m_oBox->m_iHH);
			mem->next_values[m_oBox] = QRect(m_oBox->m_iXX, m_oBox->m_iYY, m_iWW, m_iHH);
			mem->apply();
		} else if (m_iMovingRow > -1) {
			mem_size_matrix *mem = new mem_size_matrix(m_oView->m_oMediator, m_oView->m_iId);
			mem->m_oBox = m_oBox;
			mem->m_iPrevValue = m_iLastSize;
			mem->m_iNextValue = m_oBox->m_oRowSizes[m_iMovingRow];
			mem->m_bIsRow = true;
			mem->m_iIdx = m_iMovingRow;
			mem->apply();
		} else if (m_iMovingCol > -1) {
			mem_size_matrix *mem = new mem_size_matrix(m_oView->m_oMediator, m_oView->m_iId);
			mem->m_oBox = m_oBox;
			mem->m_iPrevValue = m_iLastSize;
			mem->m_iNextValue = m_oBox->m_oColSizes[m_iMovingCol];
			mem->m_bIsRow = false;
			mem->m_iIdx = m_iMovingCol;
			mem->apply();
		}
	}
	else
	{
		QGraphicsRectItem::mouseReleaseEvent(e);
	}
}

