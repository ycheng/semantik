// Thomas Nagy 2007-2015 GPLV3

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

#include <QFont>

#define PAD 2
#define MIN_FORK_SIZE 30

box_label::box_label(box_view* view, int id) : box_item(view, id)
{
	QFont font = doc.defaultFont();
	font.setPointSize(font.pointSize() - 2);
	doc.setDefaultFont(font);
}

void box_label::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	painter->save();
	doc.setDefaultFont(scene()->font());
	//painter->setFont(scene()->font());

	QRectF l_oRect = boundingRect().adjusted(PAD, PAD, -PAD, -PAD);

	if (isSelected())
	{
		QPen l_oPen = QPen(Qt::DotLine);
		l_oPen.setColor(Qt::black);
		l_oPen.setCosmetic(false);
		l_oPen.setWidth(1);
		painter->setPen(l_oPen);
		painter->drawRoundRect(l_oRect, 20, 20);

		l_oPen.setStyle(Qt::SolidLine);
		painter->setPen(l_oPen);
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

QVariant box_label::itemChange(GraphicsItemChange i_oChange, const QVariant &i_oValue)
{
	if (scene())
	{
		if (i_oChange == ItemPositionHasChanged)
		{
			update_links();
		}
		else if (i_oChange == ItemSelectedHasChanged)
		{
			m_oChain->setVisible(isSelected());
		}
	}

	return QGraphicsItem::itemChange(i_oChange, i_oValue);
}


