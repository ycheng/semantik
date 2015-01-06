// Thomas Nagy 2007-2015 GPLV3

#include <QApplication>
#include <QAbstractTextDocumentLayout>
#include <QTextDocument>
#include <QTextDocumentFragment>
#include <QAbstractTextDocumentLayout>
#include <QTextList>
#include <QClipboard>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QtDebug>
#include <QAction>
#include <QTextDocument>
#include <QLinearGradient>
#include "box_node.h"
#include "data_item.h"
 #include "res:zable.h"
#include "box_view.h"
 #include "box_link.h"
#include "sem_mediator.h"
#include "mem_box.h"

#include <QFont>

#define PAD 2
#define MIN_FORK_SIZE 30

box_node::box_node(box_view* view, int id) : box_item(view, id)
{
	QFont font = doc.defaultFont();
	font.setBold(true);
	doc.setDefaultFont(font);

	setZValue(90);
}

void box_node::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	painter->save();
	doc.setDefaultFont(scene()->font());

	QRectF l_oRect = boundingRect().adjusted(PAD, PAD, -PAD, -PAD);

	QColor bc(m_oBox->color);
	painter->setBrush(bc);

	QPen l_oPen = QPen(Qt::SolidLine);
	l_oPen.setColor(Qt::black);
	l_oPen.setCosmetic(false);
	l_oPen.setWidth(1);
	if (isSelected())
	{
		l_oPen.setStyle(Qt::DotLine);
	}

	painter->setPen(l_oPen);
	QRectF br = l_oRect.adjusted(0, 10, -10, 0);
	painter->drawRect(br);


	painter->setBrush(bc.lighter(GRADVAL));

	QPointF pts[4];
	pts[0] = br.topLeft();
	pts[1] = br.topLeft() + QPointF(10, -10);
	pts[2] = br.topRight() + QPointF(10, -10);
	pts[3] = br.topRight();

	painter->drawPolygon(pts, 4);

	painter->setBrush(bc.darker(GRADVAL));

	pts[0] = br.topRight();
	pts[1] = br.topRight() + QPointF(10, -10);
	pts[2] = br.bottomRight() + QPointF(10, -10);
	pts[3] = br.bottomRight();

	painter->drawPolygon(pts, 4);

	if (isSelected())
	{
		l_oPen.setStyle(Qt::SolidLine);
		painter->setPen(l_oPen);
		painter->setBrush(QColor("#FFFF00"));
		QRectF l_oR2(m_iWW - 10 - 8, m_iHH - 8, 6, 6);
		painter->drawRect(l_oR2);
	}

	painter->translate(OFF, OFF + 10);
	QAbstractTextDocumentLayout::PaintContext ctx;
	ctx.palette = QApplication::palette("QTextControl");
	ctx.palette.setColor(QPalette::Text, Qt::black); // white on black kde themes
	doc.documentLayout()->draw(painter, ctx);

	painter->restore();
}

void box_node::update_size() {
	m_iWW = m_oBox->m_iWW;
	m_iHH = m_oBox->m_iHH;

	doc.setHtml(QString("<div align='center'>%1</div>").arg(m_oBox->m_sText));
	doc.setTextWidth(m_iWW - 2 * OFF - 20);

	setRect(0, 0, m_iWW, m_iHH);
}

void box_node::mousePressEvent(QGraphicsSceneMouseEvent* e)
{
	m_oLastPressPoint = e->pos();
	if (m_oLastPressPoint.x() > m_iWW - GRID - 10 && m_oLastPressPoint.x() < m_iWW - 10 && m_oLastPressPoint.y() > m_iHH - GRID)
	{
		setFlags(ItemIsSelectable | ItemSendsGeometryChanges);
		m_bMoving = true;
	}
	QGraphicsRectItem::mousePressEvent(e);
}


