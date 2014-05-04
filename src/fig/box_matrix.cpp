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
		painter->setBrush(QColor("#FFFF00"));
		QRectF l_oR2(m_iWW - 8, m_iHH - 8, 6, 6);
		painter->drawRect(l_oR2);
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


