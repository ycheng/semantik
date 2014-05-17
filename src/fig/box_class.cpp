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
#include "box_class.h"
#include "data_item.h"
 #include "res:zable.h"
#include "box_view.h"
 #include "box_link.h"
#include "sem_mediator.h"
#include "mem_box.h"

#include <QFont>

#define PAD 2
#define MIN_FORK_SIZE 30

box_class::box_class(box_view* view, int id) : box_item(view, id)
{
	QFont font = doc.defaultFont();
	font.setPointSize(font.pointSize() - 2);
	doc.setDefaultFont(font);

	QSizeF l_o = size();
	setRect(0, 0, l_o.width(), l_o.height());
}

void box_class::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	painter->save();

	QRectF l_oRect = boundingRect().adjusted(PAD, PAD, -PAD, -PAD);

	QPen l_oPen = QPen(Qt::SolidLine);
	l_oPen.setColor(Qt::black);
	l_oPen.setCosmetic(false);
	l_oPen.setWidth(1);
	painter->setPen(l_oPen);



	// oh the text
	qreal l_oHpos = 0;
	QFont l_oFont(m_oView->font());
	l_oFont.setBold(true);
	painter->setFont(l_oFont);
	QFontMetricsF l_oFm(l_oFont);

	{
		QRectF l_oR = l_oFm.boundingRect(m_oBox->m_sText);
		l_oR.setWidth(l_oRect.width());
		l_oHpos += l_oR.height();
		l_oR.translate(l_oRect.topLeft() + QPointF(0, l_oHpos));
		painter->drawText(l_oR, Qt::AlignCenter | Qt::TextSingleLine | Qt::AlignVCenter, m_oBox->m_sText);
	}

	QFont l_oFont2(m_oView->font());
	l_oFont2.setBold(false);
	l_oFm = QFontMetricsF(l_oFont2);
	painter->setFont(l_oFont2);

	qreal l_iHVisibility = minVisibility(l_oFm);

	// FIXME painter->drawLine()...
	foreach (data_box_method *l_o, m_oBox->m_oMethods) {
		QRectF l_oR = l_oFm.boundingRect(l_o->m_sText);
		l_oR.setWidth(l_oRect.width());
		qreal l_fOff = 2*PAD + l_iHVisibility;
		l_oHpos += l_oR.height();
		l_oR.setWidth(l_oR.width() - l_fOff);
		l_oR.translate(l_oRect.topLeft() + QPointF(l_fOff, l_oHpos));
		painter->drawText(l_oR, Qt::AlignLeft | Qt::TextSingleLine | Qt::AlignTop, l_o->m_sText);

		QString l_sVis;
		if (l_o->m_oVisibility == visibility::PUBLIC) {
			l_sVis = "+";
		} else if (l_o->m_oVisibility == visibility::PROTECTED) {
			l_sVis = "#";
		} else if (l_o->m_oVisibility == visibility::PRIVATE) {
			l_sVis = "-";
		} else if (l_o->m_oVisibility == visibility::PACKAGE) {
			l_sVis = "~";
		} else if (l_o->m_oVisibility == visibility::DERIVED) {
			l_sVis = "/";
		} else {
			Q_ASSERT("invalid visibility " + l_o->m_oVisibility);
		}



		l_oR.translate(QPointF(PAD - l_fOff, 0));
		painter->drawText(l_oR, Qt::AlignLeft | Qt::TextSingleLine | Qt::AlignTop, l_sVis);

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

qreal box_class::minVisibility(const QFontMetricsF i_oFm)
{
	qreal l_iHVisibility = 0;
	if (i_oFm.width("+") > l_iHVisibility) {
		l_iHVisibility = i_oFm.width("+");
	}
	if (i_oFm.width("-") > l_iHVisibility) {
		l_iHVisibility = i_oFm.width("-");
	}
	if (i_oFm.width("#") > l_iHVisibility) {
		l_iHVisibility = i_oFm.width("#");
	}
	if (i_oFm.width("~") > l_iHVisibility) {
		l_iHVisibility = i_oFm.width("~");
	}
	if (i_oFm.width("/") > l_iHVisibility) {
		l_iHVisibility = i_oFm.width("/");
	}
	return l_iHVisibility;
}

QSizeF box_class::size() {
	QSizeF l_oRet;
	QFont l_oFont(m_oView->font());
	QFontMetricsF l_oFm(l_oFont);

	qreal l_iHVisibility = minVisibility(l_oFm);

	qreal l_iWW = 0, l_iHH = 1;
	foreach (data_box_method *l_o, m_oBox->m_oMethods) {
		QRectF l_oR = l_oFm.boundingRect(l_o->m_sText);
		l_iWW = qMax(l_oR.width() +  3 * PAD + l_iHVisibility, l_iWW);
		l_iHH += l_oR.height() + PAD;
	}
	foreach (data_box_attribute *l_o, m_oBox->m_oAttributes) {
		QRectF l_oR = l_oFm.boundingRect(l_o->m_sText);
		l_iWW = qMax(l_oR.width() +  3 * PAD + l_iHVisibility, l_iWW);
		l_iHH += l_oR.height() + PAD;
	}

	l_oFont.setBold(true);
	l_oFm = QFontMetricsF(l_oFont);
	{
		QRectF l_oR = l_oFm.boundingRect(m_oBox->m_sText);
		l_iWW = qMax(l_oR.width() +  2 * PAD, l_iWW);
		l_iHH += l_oR.height() + PAD;
	}

	if (l_iWW < 100) l_iWW = 100;
	if (l_iHH < 50) l_iHH = 50;

	return QSizeF(l_iWW, l_iHH);
}

