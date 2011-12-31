// Thomas Nagy 2007-2011 GPLV3

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
#include "box_item.h"
#include "box_view.h"
 #include "box_link.h"
#include "data_item.h"
#include "sem_model.h"

#define PAD 3

box_item::box_item(box_view* i_oParent, int i_iId) : QGraphicsTextItem(), m_oView(i_oParent)
{
	//l_oOpt.setAlignment(Qt::AlignCenter);
	//m_oDoc->setDefaultTextOption(l_oOpt);

	//setBrush(QColor(170, 170, 255));

	m_iId = i_iId;
	m_bSel = false;
	m_bEdit = false;

	setPlainText("");
	//adjustSize();

	//setRect(0, 0, 20, 20);
	//m_oColor = QColor("#FFFCD5");
	//m_oColor = s_oColorRoot; //"#ABFBC7");
	//m_oColorBackup = m_oColor;

	m_oItem = m_oView->m_oControl->m_oItems[m_oView->m_iId];
	m_oBox = m_oItem->m_oBoxes[m_iId];

	i_oParent->scene()->addItem(this);

	setCacheMode(QGraphicsItem::DeviceCoordinateCache);

	setZValue(64);
	setTextWidth(80);
	setFlags(ItemIsMovable | ItemIsSelectable);
}

box_item::~box_item()
{

}

void box_item::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	painter->save();

	QRectF l_oRect = boundingRect().adjusted(PAD, PAD, -PAD, -PAD);
	data_item *l_oItem = m_oItem; //m_oGraph->m_oControl->m_oItems.value(Id());

	QPen l_oPen = QPen(Qt::SolidLine);

	l_oPen.setColor(Qt::black);
	if (isSelected()) l_oPen.setStyle(Qt::DotLine);
	l_oPen.setCosmetic(true);

	painter->setPen(l_oPen);

	if (textInteractionFlags() & Qt::TextEditorInteraction)
	{
		painter->setBrush(Qt::white);
	}
	else
	{
		painter->setBrush(m_oBox->fill_color);
	}

	painter->drawRoundRect(l_oRect, 20, 20);

	QAbstractTextDocumentLayout::PaintContext ctx;
	if (textInteractionFlags() & Qt::TextEditorInteraction) {
		QTextCursor cursor = textCursor();
		ctx.cursorPosition = cursor.position();
		QAbstractTextDocumentLayout::Selection selection;
		selection.cursor = cursor;
		selection.format.setBackground(ctx.palette.brush(QPalette::Active, QPalette::Highlight));
		selection.format.setForeground(ctx.palette.brush(QPalette::Active, QPalette::HighlightedText));
		ctx.selections.append(selection);
	}

	ctx.clip = l_oRect;
	document()->documentLayout()->draw(painter, ctx);

	painter->restore();
}

/*
void box_item::paint(QPainter *i_oPainter, const QStyleOptionGraphicsItem *option, QWidget * i_oW)
{
	//QGraphicsRectItem::paint(i_oPainter, option, i_oW);
	QPen l_oPen = QPen(Qt::SolidLine);
	l_oPen.setColor(QColor(Qt::black));
	l_oPen.setWidth(1);
	if (m_oControl->m_oSelected.contains(this)) l_oPen.setWidth(2);
	i_oPainter->setPen(l_oPen);

	QRectF l_oB = boundingRect();

	qreal w = l_oPen.width()/2.;
	QRectF l_oRect = l_oB.adjusted(w, w, -w, -w);

	if (m_bEdit) i_oPainter->setBrush(QColor(255, 255, 255));
	else i_oPainter->setBrush(brush());


	i_oPainter->drawRoundRect(l_oRect, 20, 20);
	if (m_oControl->m_oSelected.contains(this) and m_oControl->m_oSelected.size() == 1)
	{
		QPointF l_oOffset(3, 3);
		i_oPainter->save();

		QPen l_oPen = i_oPainter->pen();
		l_oPen.setWidth(1);
		i_oPainter->setPen(l_oPen);

		i_oPainter->setBrush(QColor(255, 255, 0));
		QPointF l_o = l_oRect.bottomRight();
		i_oPainter->drawEllipse(QRectF(l_o - 2*l_oOffset, l_o));
		i_oPainter->restore();
	}


	i_oPainter->save();


	// previously l_oRect
	m_oDoc->setTextWidth(l_oB.width() - 14.);
	i_oPainter->translate(
		(l_oB.width() - m_oDoc->documentLayout()->documentSize().width()) / 2,
		(l_oB.height() - m_oDoc->documentLayout()->documentSize().height()) / 2);

	QAbstractTextDocumentLayout::PaintContext ctx;
	ctx.palette = QApplication::palette("QTextControl");

	if (m_bEdit)
	{
		ctx.cursorPosition = m_oCursor->position();
		if (m_oCursor->hasSelection())
		{
			QAbstractTextDocumentLayout::Selection selection;
			selection.cursor = *m_oCursor;
			QPalette::ColorGroup cg = QPalette::Active;
			selection.format.setBackground(ctx.palette.brush(cg, QPalette::Highlight));
			selection.format.setForeground(ctx.palette.brush(cg, QPalette::HighlightedText));
			ctx.selections.append(selection);
		}
	}
	else
	{
		ctx.cursorPosition = -1;
	}

	m_oDoc->documentLayout()->draw(i_oPainter, ctx);
	//m_oDoc->drawContents(i_oPainter);
	i_oPainter->restore();
}*/

/*
void box_item::set_pos(QPointF i_oP)
{
	set_pos(i_oP.x(), i_oP.y());
}

void box_item::set_pos(qreal i_iXX, qreal i_iYY)
{
	m_iXX = i_iXX;
	m_iYY = i_iYY;
	qreal l_iXX = GRID_VALUE * (int) (i_iXX / GRID_VALUE);
	qreal l_iYY = GRID_VALUE * (int) (i_iYY / GRID_VALUE);
	QGraphicsRectItem::setPos(l_iXX, l_iYY);
}

void box_item::move_by(qreal i_iXX, qreal i_iYY)
{
	set_pos(m_iXX + i_iXX, m_iYY + i_iYY);
}
*/

void box_item::mousePressEvent(QGraphicsSceneMouseEvent* e) {
	setZValue(100);
	QGraphicsTextItem::mousePressEvent(e);
}

void box_item::mouseReleaseEvent(QGraphicsSceneMouseEvent* e) {
	setZValue(99);
	QGraphicsTextItem::mouseReleaseEvent(e);
}

QRectF box_item::boundingRect() const {
	QTextDocument *doc = document();
	QRectF rect(QPointF(0, 0), doc->size());
	return rect.adjusted(-OFF, -OFF, OFF, OFF);
}

void box_item::update_data() {
	setPos(QPointF(m_oBox->m_iXX, m_oBox->m_iYY));
	setPlainText(m_oBox->m_sText);
}

void box_item::keyPressEvent(QKeyEvent* e) {
	// FIXME Qt Sucks
	if (e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return)
	{
		m_oView->m_oEditAction->activate(QAction::Trigger);
		return;
	}
	else if (e->key() == Qt::Key_Escape)
	{
		m_oView->m_oCancelEditAction->activate(QAction::Trigger);
		return;
	}


	QGraphicsTextItem::keyPressEvent(e);
	update();
}

void box_item::keyReleaseEvent(QKeyEvent* e) {
	if (e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return || e->key() == Qt::Key_Escape)
	{
		e->ignore();
		return;
	}
	QGraphicsTextItem::keyReleaseEvent(e);
}

QVariant box_item::itemChange(GraphicsItemChange i_oChange, const QVariant &i_oValue)
{
	bool l_bChange = ((i_oChange == ItemPositionChange) && scene());

	QVariant l_oRet = QGraphicsItem::itemChange(i_oChange, i_oValue);

	if (l_bChange)
	{
		qDebug()<<"stuff changed, update links";
		update_links();
	}

	return l_oRet;
}

void box_item::update_links()
{
	foreach (box_link* l_oLink, m_oView->m_oLinks)
	{
		l_oLink->update_pos();
	}
}

