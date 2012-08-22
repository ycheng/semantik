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
#include "box_label.h"
#include "data_item.h"
 #include "res:zable.h"
#include "box_view.h"
 #include "box_link.h"
#include "sem_mediator.h"
#include "mem_box.h"


#define PAD 2
#define MIN_FORK_SIZE 30

void box_label::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	painter->save();

	QRectF l_oRect = boundingRect().adjusted(PAD, PAD, -PAD, -PAD);

	if (isSelected())
	{
		QPen l_oPen = QPen(Qt::DotLine);
		l_oPen.setColor(Qt::black);
		l_oPen.setCosmetic(false);
		l_oPen.setWidth(1);
		painter->setPen(l_oPen);
		painter->drawRect(l_oRect);
	}

	painter->translate(OFF, OFF);
	QAbstractTextDocumentLayout::PaintContext ctx;
	ctx.palette = QApplication::palette("QTextControl");
	doc.documentLayout()->draw(painter, ctx);

	painter->restore();
}

