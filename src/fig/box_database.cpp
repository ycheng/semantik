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
	//l_oPen.setJoinStyle(Qt::RoundJoin);
	painter->setPen(l_oPen);

	QRectF l_oRect2(l_oRect);
	qreal l_fRat = l_oRect2.width()/2.2;
	if (l_fRat > l_oRect.height() - 5)
	{
		l_fRat = l_oRect.height() - 5;
	}
	l_oRect2.setHeight(l_fRat);
	painter->drawEllipse(l_oRect2);

	QRectF l_oRect3 = l_oRect2.translated(0, l_oRect.height() - l_oRect2.height());
	painter->drawArc(l_oRect3, 180*16, 180*16);

	qreal l_fY1 = l_oRect2.topLeft().y() + l_oRect2.height()/2.;
	qreal l_fY2 = l_oRect3.topLeft().y() + l_oRect3.height()/2.;

	qreal l_fX1 = l_oRect.topLeft().x();
	qreal l_fX2 = l_oRect.topRight().x();
	painter->drawLine(l_fX1, l_fY1, l_fX1, l_fY2);
	painter->drawLine(l_fX2, l_fY1, l_fX2, l_fY2);


	if (isSelected())
	{
		painter->setBrush(QColor("#FFFF00"));
		QRectF l_oR2(m_iWW - 8, m_iHH - 8, 6, 6);
		painter->drawRect(l_oR2);
	}

	painter->restore();
}

