// Thomas Nagy 2007-2011 GPLV3

#include <QApplication>
#include <QAbstractTextDocumentLayout>
#include <QTextDocument>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QClipboard>
#include <QPainter>
#include <QStyleOption>
#include <QColor>
#include <QTextCursor>
#include <QPen>
#include <QtDebug>
#include <QKeyEvent>
#include <QTextList>
#include <QTextBlock>
#include <QSvgRenderer>
#include <QTextDocumentFragment>

#include "canvas_flag.h"
#include "color_scheme.h"
#include "sem_model.h"
#include "con.h" 
#include "data_item.h"
#include "canvas_sort.h"
#include "canvas_link.h"
#include "canvas_item.h"
#include "canvas_view.h"

#define square_size 5
#define item_padding 7.

QColor canvas_item::s_oColorRect   = QColor("#00CCCC"); // QColor(0, 0, 255, 255);
QColor canvas_item::s_oColorRectEd = QColor("#ee6060");
QColor canvas_item::s_oColorRoot   = QColor("#FFFCD5");
QColor canvas_item::s_oColorLeaf   = QColor(171, 251, 199);
QColor canvas_item::s_oColorSelect = QColor(255, 255, 255, 240);

canvas_item::canvas_item(canvas_view *i_oGraphWidget, int i_iId) : QGraphicsTextItem(), m_oGraph(i_oGraphWidget)
{
	//QTextOption l_oOpt;
	//l_oOpt.setWrapMode(QTextOption::WordWrap);
	//setDefaultTextOption(l_oOpt);

	m_iId = i_iId;
	m_bEdit = false;
	m_sNum = "1";

	setPlainText(m_oGraph->m_oControl->m_oItems.value(Id())->m_sSummary);
	//adjustSize();

	m_oSort = new canvas_sort(i_oGraphWidget, this);

	setFlags(ItemIsMovable | ItemIsSelectable);

	setZValue(100);
	//setRect(0, 0, 20, 20);
	//m_oColor = QColor("#FFFCD5");
	m_oColor = s_oColorRoot; //"#ABFBC7");
	m_oColorBackup = m_oColor;

	i_oGraphWidget->scene()->addItem(this);
	m_oSort->hide();

	update_flags();

	setCacheMode(QGraphicsItem::DeviceCoordinateCache);
}

// (i_oEv == QKeySequence::DeleteEndOfWord)

//void canvas_item::keyReleaseEvent(QKeyEvent* i_oEv)
// i_oEv->key() == Qt::Key_Enter || i_oEv->key() == Qt::Key_Return)

void canvas_item::set_parent(canvas_item * i_o)
{
	m_oParent = i_o;
}

QVariant canvas_item::itemChange(GraphicsItemChange i_oChange, const QVariant &i_oValue)
{
	bool l_bChange = ((i_oChange == ItemPositionChange) && scene());

	QVariant l_oRet = QGraphicsItem::itemChange(i_oChange, i_oValue);

	if (l_bChange)
	{
		update_links();
	}

	return l_oRet;
}

void canvas_item::add_link(canvas_link* i_oL)
{
	update_color();
	m_oLinks.push_back(i_oL);
}

void canvas_item::rm_link(canvas_link* i_oLink2)
{
	update_color();
	m_oLinks.removeAll(i_oLink2);
}

void canvas_item::update_data()
{
	data_item *l_oItem = m_oGraph->m_oControl->m_oItems.value(Id());
	if (l_oItem->m_iXX < -10000.0) {
		l_oItem->m_iXX = m_oGraph->m_oLastPoint.x();
		l_oItem->m_iYY = m_oGraph->m_oLastPoint.y();
	}

	setPos(QPointF(l_oItem->m_iXX, l_oItem->m_iYY));
	setPlainText(l_oItem->m_sSummary);
	adjustSize();
	update_links();
	update_flags();
}

void canvas_item::update_color()
{
	int l_oP = m_oGraph->m_oControl->parent_of(Id());
	if (l_oP < 0) m_oColor = s_oColorRoot;
	else m_oColor = s_oColorLeaf;
	m_oColorBackup = m_oColor;
}

void canvas_item::update_links()
{
	foreach (canvas_link* l_oLink, m_oLinks)
	{
		l_oLink->update_pos();
	}
}

void canvas_item::update_flags()
{
	data_item *l_oItem = m_oGraph->m_oControl->m_oItems.value(Id());
	int l_iDiff = l_oItem->m_oFlags.size() - m_oFlags.size();

	while (l_iDiff > 0)
	{
		l_iDiff--;
		canvas_flag *l_o = new canvas_flag(m_oGraph, this);
		m_oFlags.push_back(l_o);
		l_o->show();
	}

	while (l_iDiff < 0)
	{
		l_iDiff++;
		canvas_flag *l_o = m_oFlags.takeFirst();
		l_o->hide();
		delete l_o;
	}

	qreal l_iWidth = textWidth();
	for (int i=0; i<m_oFlags.size(); ++i)
	{
		canvas_flag *l_o = m_oFlags[i];
		QPointF l_oF = QPointF(l_iWidth - 25*i - 16, -22);
		l_o->setPos(l_oF);
	}
}

canvas_item::~canvas_item()
{
}

void canvas_item::mousePressEvent(QGraphicsSceneMouseEvent* e) {
	setZValue(100);
	QGraphicsTextItem::mousePressEvent(e);
}

void canvas_item::mouseReleaseEvent(QGraphicsSceneMouseEvent* e) {
	setZValue(99);
	QGraphicsTextItem::mouseReleaseEvent(e);
}

void canvas_item::keyPressEvent(QKeyEvent* e) {
	// FIXME Qt Sucks
	if (e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return)
	{
		m_oGraph->m_oEditAction->activate(QAction::Trigger);
		return;
	}
	else if (e->key() == Qt::Key_Escape)
	{
		m_oGraph->m_oCancelEditAction->activate(QAction::Trigger);
		return;
	}

	QGraphicsTextItem::keyPressEvent(e);
	adjustSize();
	update_links();
}

void canvas_item::keyReleaseEvent(QKeyEvent* e) {
	if (e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return || e->key() == Qt::Key_Escape)
	{
		return;
	}
	QGraphicsTextItem::keyReleaseEvent(e);
}

QRectF canvas_item::boundingRect() const {
	QTextDocument *doc = document();
	QRectF rect(QPointF(0, 0), doc->size());
	return rect.adjusted(-OFF, -OFF, OFF, OFF);
}

QPainterPath canvas_item::opaqueArea() const
{
	return shape();
}

void canvas_item::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	painter->save();

	QRectF l_oRect = boundingRect().adjusted(PAD, PAD, -PAD, -PAD);
	data_item *l_oItem = m_oGraph->m_oControl->m_oItems.value(Id());
	color_scheme l_oColorScheme = l_oItem->get_color_scheme();

	QPen l_oPen = QPen(Qt::SolidLine);

	l_oPen.setColor(l_oColorScheme.m_oBorderColor);
	if (isSelected()) l_oPen.setStyle(Qt::DotLine);
	l_oPen.setCosmetic(true);

	painter->setPen(l_oPen);

	if (textInteractionFlags() & Qt::TextEditorInteraction)
	{
		painter->setBrush(Qt::white);
	}
	else
	{
		if (m_oGraph->m_oControl->parent_of(Id()) <= 0 && l_oItem->m_iColor > 1)
		{
			QLinearGradient l_oGradient(l_oRect.right()-40, 0, l_oRect.right()-10, 0);
			l_oGradient.setColorAt(0., l_oColorScheme.m_oInnerColor);
			l_oGradient.setColorAt(1., l_oItem->get_color_scheme_raw().m_oInnerColor);

			QBrush l_oBrush(l_oGradient);
			painter->setBrush(l_oBrush);
		}
		else
		{
			painter->setBrush(l_oColorScheme.m_oInnerColor);
		}
	}

	painter->drawRoundRect(l_oRect, 20, 20);

	// if there is text, draw a triangle on the top-right corner
	if (m_oGraph->m_oControl->m_oItems.value(Id())->m_sText.length() > 0)
	{
		const QPointF points[4] =
		{
			l_oRect.topRight(),
			l_oRect.topRight()-QPointF(5, 0),
			l_oRect.topRight()+QPointF(0, 5),
		};
		painter->setBrush(l_oColorScheme.m_oBorderColor);
		painter->drawPolygon(points, 3);
		painter->setBrush(l_oColorScheme.m_oInnerColor);
	}
	//painter->translate(5, 5);


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

