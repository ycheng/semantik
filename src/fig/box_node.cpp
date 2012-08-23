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
	setZValue(90);
}

void box_node::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	painter->save();

	QRectF l_oRect = boundingRect().adjusted(PAD, PAD, -PAD, -PAD);

	painter->setBrush(m_oBox->color);

	QPen l_oPen = QPen(Qt::SolidLine);
	l_oPen.setColor(Qt::black);
	l_oPen.setCosmetic(false);
	l_oPen.setWidth(1);
	if (isSelected())
	{
		l_oPen.setStyle(Qt::DotLine);
	}

	painter->setPen(l_oPen);
	QRectF br = l_oRect.adjusted(0, 0, -20, -20);
	painter->drawRect(br);

	QPointF pts[4];
	pts[0] = br.bottomRight();
	pts[1] = br.bottomRight() + QPointF(20, 20);
	pts[2] = br.bottomLeft() + QPointF(20, 20);
	pts[3] = br.bottomLeft();

	painter->drawPolygon(pts, 4);

	pts[0] = br.topRight();
	pts[1] = br.topRight() + QPointF(20, 20);
	pts[2] = br.bottomRight() + QPointF(20, 20);
	pts[3] = br.bottomRight();

	painter->drawPolygon(pts, 4);

	if (isSelected())
	{
		painter->setBrush(QColor("#FFFF00"));
		QRectF l_oR2(m_iWW - 8, m_iHH - 8, 6, 6);
		painter->drawRect(l_oR2);
	}

	painter->translate(OFF, OFF);
	QAbstractTextDocumentLayout::PaintContext ctx;
	ctx.palette = QApplication::palette("QTextControl");
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

