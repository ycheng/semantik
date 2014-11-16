// Thomas Nagy 2007-2014 GPLV3

#include <QApplication>
#include <QAbstractTextDocumentLayout>
#include <QTextDocument>
#include <QTextDocumentFragment>
#include <QTextList>
#include <QClipboard>
#include <QPainter>
#include <QtDebug>
#include <QAction>
#include <QTextDocument>
#include "box_database.h"
#include "data_item.h"
 #include "res:zable.h"
#include "box_view.h"
 #include "box_link.h"
#include "sem_mediator.h"
#include "mem_box.h"

#include <QFont>

#define PAD 2
#define MIN_FORK_SIZE 30

box_database::box_database(box_view* view, int id) : box_item(view, id)
{
	QFont font = doc.defaultFont();
	font.setPointSize(font.pointSize() - 2);
	doc.setDefaultFont(font);
}

void box_database::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	painter->save();

	QRectF l_oRect = boundingRect().adjusted(PAD, PAD, -PAD, -PAD);

	QPen l_oPen = QPen(Qt::SolidLine);
	l_oPen.setColor(Qt::black);
	l_oPen.setCosmetic(false);
	l_oPen.setWidth(1);
	painter->setPen(l_oPen);

	painter->drawRect(l_oRect);

	/*
	qreal xtop = l_oRect.x();
	qreal ytop = l_oRect.y();
	qreal xcoord = xtop + l_oRect.width() / 2.0;
	qreal ycoord = l_oRect.height() / 5.;

	painter->drawLine(QLineF(xcoord, ytop + 2 * ycoord, xcoord, ytop + 4 * ycoord));
	painter->drawLine(QLineF(xtop, ytop + 3 * ycoord, xtop + l_oRect.width(), ytop + 3 * ycoord));

	painter->drawLine(QLineF(xtop, ytop + l_oRect.height(), xcoord, ytop + 4 * ycoord));
	painter->drawLine(QLineF(xtop + l_oRect.width(), ytop + l_oRect.height(), xcoord, ytop + 4 * ycoord));

	double cir = 4 * l_oRect.width() / 10.;
	if (cir > ycoord) {
		cir = ycoord;
	}
	painter->drawEllipse(QRectF(xcoord - cir, ytop + 2 * ycoord - 2 * cir, 2 * cir, 2 * cir));

	*/

	if (isSelected())
	{
		painter->setBrush(QColor("#FFFF00"));
		QRectF l_oR2(m_iWW - 8, m_iHH - 8, 6, 6);
		painter->drawRect(l_oR2);
	}

	painter->restore();
}

