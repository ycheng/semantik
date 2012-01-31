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
#include "box_item.h"
#include "box_view.h"
 #include "box_link.h"
#include "data_item.h"
#include "sem_mediator.h"

#define PAD 2
#define GRID 10

box_item::box_item(box_view* i_oParent, int i_iId) : QGraphicsTextItem(), m_oView(i_oParent), resizable(), connectable()
{
	m_iId = i_iId;
	setPlainText("");
	//adjustSize();

	//setRect(0, 0, 20, 20);

	m_oItem = m_oView->m_oMediator->m_oItems[m_oView->m_iId];
	m_oBox = m_oItem->m_oBoxes[m_iId];
	Q_ASSERT(m_oBox);

	i_oParent->scene()->addItem(this);

	setCacheMode(QGraphicsItem::DeviceCoordinateCache);

	setZValue(100);
	setTextWidth(80);
	setFlags(ItemIsMovable | ItemIsSelectable | ItemSendsGeometryChanges);
}

box_item::~box_item()
{

}

void box_item::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	painter->save();

	QRectF l_oRect = boundingRect().adjusted(PAD, PAD, -PAD, -PAD);
	data_item *l_oItem = m_oItem; //m_oGraph->m_oMediator->m_oItems.value(Id());

	QPen l_oPen = QPen(Qt::SolidLine);

	l_oPen.setColor(Qt::black);
	if (isSelected()) l_oPen.setStyle(Qt::DotLine);
	l_oPen.setCosmetic(false);
	l_oPen.setWidth(1);

	painter->setPen(l_oPen);

	if (textInteractionFlags() & Qt::TextEditorInteraction)
	{
		painter->setBrush(Qt::white);
	}
	else
	{
		painter->setBrush(m_oBox->color);
	}

	painter->drawRoundRect(l_oRect, 20, 20);

	QAbstractTextDocumentLayout::PaintContext ctx;
	ctx.palette = QApplication::palette("QTextControl");
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

	// TODO resize handle
}

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
	update_links();
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
	if (scene())
	{
		if (i_oChange == ItemPositionChange)
		{
			QPointF np = i_oValue.toPointF();
			np.setX(((int) np.x() / GRID) * GRID);
			np.setY(((int) np.y() / GRID) * GRID);
			return np;
		}
		else if (i_oChange == ItemPositionHasChanged)
		{
			update_links();
		}
		else if (i_oChange == ItemSelectedHasChanged)
		{
			if (isSelected())
				setZValue(101);
			else
				setZValue(100);
		}
	}

	return QGraphicsItem::itemChange(i_oChange, i_oValue);
}

void box_item::update_links()
{
	// FIXME
	foreach (box_link* l_oLink, m_oView->m_oLinks)
	{
		l_oLink->update_pos();
	}
}

static int RATIO[] = {333, 500, 667, 0};
#define MUL 64

int box_item::choose_position(const QPointF& i_oP, int id)
{
	QRectF r = rect();
	QPointF l_o = pos() - i_oP + QPointF(r.width()/2, r.height()/2);
	double c_x = l_o.x() * r.height();
	double c_y = l_o.y() * r.width();

	int ret = 0;
	int best = 1<<30;
	int cand = 0;
	if (qAbs(c_x) > qAbs(c_y))
	{
		ret = (c_x > 0) ? data_link::WEST : data_link::EAST;
		for (int i=0; i < 10; ++i) {
			int k = RATIO[i];
			if (k == 0) break;
			int val = qAbs((k * r.height() / 1000.) - (i_oP.y() - pos().y()));

			if (val < best)
			{
				best = val;
				cand = k;
			}
		}
		ret += cand * MUL;
	}
	else
	{
		ret = (c_y > 0) ? data_link::NORTH : data_link::SOUTH;
		for (int i=0; i < 10; ++i) {
			int k = RATIO[i];
			if (k == 0) break;
			int val = qAbs((k * r.width() / 1000.) - (i_oP.x() - pos().x()));

			if (val < best)
			{
				best = val;
				cand = k;
			}
		}
		ret += cand * MUL;
	}
	return ret;
}

QPoint box_item::get_point(int i_oP)
{
	QRectF r = rect();
	int ratio = i_oP / MUL;

	if (ratio >= 1000 || ratio <= 0) ratio = 500;
	switch (i_oP & data_link::COORD) {
		case data_link::NORTH:
			return QPoint(r.x() + r.width() * ratio / 1000., r.y());
		case data_link::WEST:
			return QPoint(r.x(), r.y() + r.height() * ratio / 1000.);
		case data_link::SOUTH:
			return QPoint(r.x() + r.width() * ratio / 1000., r.y() + r.height());
		case data_link::EAST:
			return QPoint(r.x() + r.width(), r.y() + r.height() * ratio / 1000.);
	}
	Q_ASSERT(false);
	return QPoint(0, 0);
}

