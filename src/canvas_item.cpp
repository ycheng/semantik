// Thomas Nagy 2007-2009 GPLV3

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

canvas_item::canvas_item(canvas_view *i_oGraphWidget, int i_iId) : QGraphicsRectItem(), m_oGraph(i_oGraphWidget)
{
	m_oDoc = new QTextDocument();

/* stupid word wrap */
#if QT_VERSION >= 0x040290
	QTextOption l_oOpt;
	l_oOpt.setWrapMode(QTextOption::WordWrap);
	m_oDoc->setDefaultTextOption(l_oOpt);
#else
	#warning "Using Qt 4.3 is recommended"
#endif

	m_oCursor = new QTextCursor(m_oDoc);

	m_iId = i_iId;
	m_bSel = false;
	m_bEdit = false;
	m_sNum = "1";

	m_oDoc->setPlainText(m_oGraph->m_oControl->m_oItems.value(Id())->m_sSummary);
	adjustSize();

	m_oSort = new canvas_sort(i_oGraphWidget, this);

	//setFlags(ItemIsMovable | ItemIsSelectable | ItemIsFocusable);
	//setFlags(ItemIsMovable | ItemIsSelectable);
	setFlags(ItemIsMovable);


	setZValue(100);
	//setRect(0, 0, 20, 20);
	//m_oColor = QColor("#FFFCD5");
	m_oColor = s_oColorRoot; //"#ABFBC7");
	m_oColorBackup = m_oColor;

	i_oGraphWidget->scene()->addItem(this);
	m_oSort->hide();

	update_flags();
}

bool canvas_item::moveKey(QKeyEvent* i_oEv)
{
	//const QTextCursor oldSelection = cursor;
	//const int oldCursorPos = m_oCursor->position();

	QTextCursor::MoveMode mode = QTextCursor::MoveAnchor;
	QTextCursor::MoveOperation op = QTextCursor::NoMove;
	if (i_oEv == QKeySequence::SelectAll)
	{
		m_oCursor->select(QTextCursor::Document);
		return true;
	}
	if (i_oEv == QKeySequence::MoveToNextChar)
	{
		op = QTextCursor::Right;
	}
	else if (i_oEv == QKeySequence::MoveToPreviousChar)
	{
		op = QTextCursor::Left;
	}
	else if (i_oEv == QKeySequence::SelectNextChar)
	{
		op = QTextCursor::Right;
		mode = QTextCursor::KeepAnchor;
	}
	else if (i_oEv == QKeySequence::SelectPreviousChar)
	{
		op = QTextCursor::Left;
		mode = QTextCursor::KeepAnchor;
	}
	else if (i_oEv == QKeySequence::SelectNextWord)
	{
		op = QTextCursor::WordRight;
		mode = QTextCursor::KeepAnchor;
	}
	else if (i_oEv == QKeySequence::SelectPreviousWord)
	{
		op = QTextCursor::WordLeft;
		mode = QTextCursor::KeepAnchor;
	}
	else if (i_oEv == QKeySequence::SelectStartOfLine)
	{
		op = QTextCursor::StartOfLine;
		mode = QTextCursor::KeepAnchor;
	}
	else if (i_oEv == QKeySequence::SelectEndOfLine)
	{
		op = QTextCursor::EndOfLine;
		mode = QTextCursor::KeepAnchor;
	}
	else if (i_oEv == QKeySequence::SelectStartOfBlock)
	{
		op = QTextCursor::StartOfBlock;
		mode = QTextCursor::KeepAnchor;
	}
	else if (i_oEv == QKeySequence::SelectEndOfBlock)
	{
		op = QTextCursor::EndOfBlock;
		mode = QTextCursor::KeepAnchor;
	}
	else if (i_oEv == QKeySequence::SelectStartOfDocument)
	{
		op = QTextCursor::Start;
		mode = QTextCursor::KeepAnchor;
	}
	else if (i_oEv == QKeySequence::SelectEndOfDocument)
	{
		op = QTextCursor::End;
		mode = QTextCursor::KeepAnchor;
	}
	else if (i_oEv == QKeySequence::SelectPreviousLine)
	{
		op = QTextCursor::Up;
		mode = QTextCursor::KeepAnchor;
	}
#if 0
	else if (e == QKeySequence::SelectNextLine) {
		op = QTextCursor::Down;
		mode = QTextCursor::KeepAnchor;
		{
			QTextBlock block = cursor.block();
			QTextLine line = currentTextLine(cursor);
			if (!block.next().isValid()
					&& line.isValid()
					&& line.lineNumber() == block.layout()->lineCount() - 1)
				op = QTextCursor::End;
		}
	}
	else if (e == QKeySequence::SelectNextLine) {
		op = QTextCursor::Down;
		mode = QTextCursor::KeepAnchor;
		{
			QTextBlock block = cursor.block();
			QTextLine line = currentTextLine(cursor);
			if (!block.next().isValid()
					&& line.isValid()
					&& line.lineNumber() == block.layout()->lineCount() - 1)
				op = QTextCursor::End;
		}
	}
#endif
	else if (i_oEv == QKeySequence::MoveToNextWord)
	{
		op = QTextCursor::WordRight;
	}
	else if (i_oEv == QKeySequence::MoveToPreviousWord)
	{
		op = QTextCursor::WordLeft;
	}
	else if (i_oEv == QKeySequence::MoveToEndOfBlock)
	{
		op = QTextCursor::EndOfBlock;
	}
	else if (i_oEv == QKeySequence::MoveToStartOfBlock)
	{
		op = QTextCursor::StartOfBlock;
	}
	else if (i_oEv == QKeySequence::MoveToNextLine)
	{
		op = QTextCursor::Down;
	}
	else if (i_oEv == QKeySequence::MoveToPreviousLine)
	{
		op = QTextCursor::Up;
	}
	else if (i_oEv == QKeySequence::MoveToPreviousLine)
	{
		op = QTextCursor::Up;
	}
	else if (i_oEv == QKeySequence::MoveToStartOfLine)
	{
		op = QTextCursor::StartOfLine;
	}
	else if (i_oEv == QKeySequence::MoveToEndOfLine)
	{
		op = QTextCursor::EndOfLine;
	}
	else if (i_oEv == QKeySequence::MoveToStartOfDocument)
	{
		op = QTextCursor::Start;
	}
	else if (i_oEv == QKeySequence::MoveToEndOfDocument)
	{
		op = QTextCursor::End;
	}
	else
	{
		return false;
	}

	/*const bool moved =*/ m_oCursor->movePosition(op, mode);
#if 0
	//m_oCursor->ensureCursorVisible();

	if (moved)
	  {
	  if (m_oCursor->position() != oldCursorPos)
	  emit q->cursorPositionChanged();
	  emit q->microFocusChanged();
	  }
#endif

	return true;
}

void canvas_item::keyPressEvent(QKeyEvent* i_oEv)
{
	// imitates qt

	if (moveKey(i_oEv)) goto accept;

	if (i_oEv->key() == Qt::Key_Backspace && !i_oEv->modifiers())
	{
		QTextBlockFormat blockFmt = m_oCursor->blockFormat();
		QTextList *list = m_oCursor->currentList();
		if (list && m_oCursor->atBlockStart())
		{
			list->remove(m_oCursor->block());
		}
		else if (m_oCursor->atBlockStart() && blockFmt.indent() > 0)
		{
			blockFmt.setIndent(blockFmt.indent() - 1);
			m_oCursor->setBlockFormat(blockFmt);
		}
		else
		{
			m_oCursor->deletePreviousChar();
		}
		goto accept;
	}

	if (i_oEv == QKeySequence::Undo)
	{
		m_oDoc->undo();
	}
	else if (i_oEv == QKeySequence::Redo)
	{
		m_oDoc->redo();
	}
	else if (i_oEv == QKeySequence::Copy)
	{
		if (m_oCursor->hasSelection())
		{
			const QTextDocumentFragment l_oF(*m_oCursor);
			QApplication::clipboard()->setText(l_oF.toPlainText());
		}
	}
	else if (i_oEv == QKeySequence::Cut)
	{
		if (m_oCursor->hasSelection())
		{
			const QTextDocumentFragment l_oF(*m_oCursor);
			QApplication::clipboard()->setText(l_oF.toPlainText());
			m_oCursor->removeSelectedText();
		}
	}
	else if (i_oEv == QKeySequence::Paste)
	{
		const QMimeData *l_oSource = QApplication::clipboard()->mimeData();
		if (l_oSource)
		{
			QString l_sText = l_oSource->text();
			if (!l_sText.isNull())
			{
				m_oCursor->insertFragment(QTextDocumentFragment::fromPlainText(l_sText));
			}
		}
	}
	else if (i_oEv == QKeySequence::Delete)
	{
		m_oCursor->deleteChar();
	}
	else if (i_oEv == QKeySequence::DeleteEndOfWord)
	{
		m_oCursor->movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor);
		m_oCursor->deleteChar();
	}
	else if (i_oEv == QKeySequence::DeleteStartOfWord)
	{
		m_oCursor->movePosition(QTextCursor::PreviousWord, QTextCursor::KeepAnchor);
		m_oCursor->deleteChar();
	}
#if 0
	else if (i_oEv == QKeySequence::DeleteEndOfLine)
	{
		QTextBlock block = cursor.block();
		if (cursor.position() == block.position() + block.length() - 2)
			cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
		else
			cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
		cursor.deleteChar();
	}
#endif
	else
	{
		goto process;
	}
	goto accept;

	process:
	{
		bool overwriteMode = false;
		QString text = i_oEv->text();
		if (!text.isEmpty() && (text.at(0).isPrint() || text.at(0) == QLatin1Char('\t')))
		{
			if (overwriteMode && !m_oCursor->hasSelection() && !m_oCursor->atBlockEnd())
				m_oCursor->deleteChar();

			m_oCursor->insertText(text);
			//selectionChanged();
		}
		else
		{
			i_oEv->ignore();
			return;
		}
	}

accept:

	i_oEv->accept();

	adjustSize();
	update();

	//QGraphicsTextItem::keyPressEvent(i_oEv);
	update_links();
}

void canvas_item::keyReleaseEvent(QKeyEvent* i_oEv)
{
	//ensureCursorVisible();
	//m_oCursor->movePosition(QTextCursor::End);

	if (i_oEv->key() == Qt::Key_Enter || i_oEv->key() == Qt::Key_Return)
	{
		return;
	}
	//QGraphicsTextItem::keyReleaseEvent(i_oEv);
}

void canvas_item::focus_in()
{
	if (m_oDoc->toPlainText() == QObject::trUtf8("Empty")) m_oDoc->setPlainText("");
	m_bEdit = true;
	m_oColorBackup = m_oColor;
	m_oColor = s_oColorSelect;
	//QKeyEvent l_oKeyEvent = QKeyEvent(QEvent::KeyPress, Qt::Key_End, Qt::NoModifier);
	//keyPressEvent(&l_oKeyEvent);
	update();
}

void canvas_item::focus_out(QFocusEvent *i_oEv)
{
	if (!m_bEdit) return;

	m_oColor = m_oColorBackup;
	m_bEdit = false;

	if (m_oDoc->toPlainText().isEmpty()) m_oDoc->setPlainText(QObject::trUtf8("Empty"));

	data_item *l_o = m_oGraph->m_oControl->m_oItems.value(Id());
	QString l_s = m_oDoc->toPlainText();
	if (l_o->m_sSummary != l_s) m_oGraph->m_oControl->set_dirty();
	l_o->m_sSummary = l_s;
	m_oGraph->m_oControl->update_item(Id(), VIEW_CANVAS);

	//QGraphicsTextItem::focusOutEvent(i_oEv);
	adjustSize();
	update_links();
	update();
}

void canvas_item::adjustSize()
{
	m_oDoc->adjustSize();
	QSizeF l_o = m_oDoc->size();
	QRectF l_oR = boundingRect();

	data_item *l_oItem = m_oGraph->m_oControl->m_oItems.value(Id());

	l_o.setWidth(qMax(l_o.width(), (qreal) l_oItem->m_oThumbnail.width()));
	l_o.setHeight(l_o.height() + l_oItem->m_oThumbnail.height());

	l_o.setWidth(l_o.width() + 2 * item_padding);
	l_o.setHeight(l_o.height() + 2 * item_padding + 2.);

	setRect(l_oR.x(), l_oR.y(), l_o.width(), l_o.height());
}

void canvas_item::set_parent(canvas_item * i_o)
{
	m_oParent = i_o;
}

void canvas_item::paint(QPainter *i_oPainter, const QStyleOptionGraphicsItem *option, QWidget * i_oW)
{
	data_item *l_oItem = m_oGraph->m_oControl->m_oItems.value(Id());
	color_scheme l_oColorScheme = l_oItem->get_color_scheme();

	QPen l_oPen = QPen(Qt::SolidLine);

	l_oPen.setColor(l_oColorScheme.m_oBorderColor);
	if (m_bSel) l_oPen.setWidth(2);
	else l_oPen.setWidth(1);

	i_oPainter->setPen(l_oPen);


	//m_oRenderer = new QSvgRenderer(QLatin1String("/home/waf/truc.svg"));

	QRectF l_oB = boundingRect();
	qreal w = l_oPen.width()/2.;
	QRectF l_oRect = l_oB.adjusted(w, w, -w, -w);

	if (m_oGraph->m_oControl->parent_of(Id()) <= 0 && l_oItem->m_iColor > 1)
	{
		QLinearGradient l_oGradient(l_oRect.right()-40, 0, l_oRect.right()-10, 0);
		l_oGradient.setColorAt(0., l_oColorScheme.m_oInnerColor);
		l_oGradient.setColorAt(1., l_oItem->get_color_scheme_raw().m_oInnerColor);

		QBrush l_oBrush(l_oGradient);
		i_oPainter->setBrush(l_oBrush);
	}
	else
	{
		i_oPainter->setBrush(l_oColorScheme.m_oInnerColor);
	}

	if (m_bEdit) i_oPainter->setBrush(QColor(255, 255, 255));

	//i_oPainter->drawRect(l_oRect);
	i_oPainter->drawRoundRect(l_oRect, 40, 40);


	//i_oPainter->drawRoundRect(boundingRect(), 2, 2);

	// if there is text, draw a triangle on the top-right corner
	if (l_oItem->m_iTextLength > 0)
	{
		const QPointF points[4] =
		{
			l_oRect.topRight(),
			l_oRect.topRight()-QPointF(5, 0),
			l_oRect.topRight()+QPointF(0, 5),
		};
		i_oPainter->setBrush(l_oColorScheme.m_oBorderColor);
		i_oPainter->drawPolygon(points, 3);
		i_oPainter->setBrush(l_oColorScheme.m_oInnerColor);
	}

	// now draw the text

	//m_oRenderer->render(i_oPainter, l_oRect);
	//i_oPainter->drawEllipse(l_oRect);

	if (!l_oItem->m_oThumbnail.isNull())
	{
		i_oPainter->save();
		i_oPainter->translate(l_oB.width()/2 - l_oItem->m_oThumbnail.width() / 2, item_padding);
		i_oPainter->drawPixmap(0, 0, l_oItem->m_oThumbnail.width(), l_oItem->m_oThumbnail.height(), l_oItem->m_oPix);
		i_oPainter->restore();
	}


	i_oPainter->save();
        i_oPainter->translate(item_padding, item_padding + l_oItem->m_oThumbnail.height() + 2.);

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

bool canvas_item::hasFocus() const
{
	return m_bEdit;
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
	setPos(QPointF(l_oItem->m_iXX, l_oItem->m_iYY));
	m_oDoc->setPlainText(l_oItem->m_sSummary);
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

	qreal l_iWidth = m_oDoc->textWidth();
	for (int i=0; i<m_oFlags.size(); ++i)
	{
		canvas_flag *l_o = m_oFlags[i];
		QPointF l_oF = QPointF(l_iWidth - 25*i - 16, -22);
		l_o->setPos(l_oF);
	}
}

canvas_item::~canvas_item()
{
	delete m_oDoc;
	delete m_oCursor;
}

void canvas_item::set_selected(bool i_b)
{
	m_bSel = i_b;
	foreach (canvas_link* l_oLink, m_oLinks)
	{
		l_oLink->update_selected();
	}
}

void canvas_item::inputMethodEvent(QInputMethodEvent *e)
{
	if (!m_bEdit)
	{
		e->ignore();
		return;
	}
	m_oCursor->beginEditBlock();
	m_oCursor->removeSelectedText();

	// insert commit string
	if (!e->commitString().isEmpty() || e->replacementLength())
	{
		QTextCursor c = *m_oCursor;
		c.setPosition(c.position() + e->replacementStart());
		c.setPosition(c.position() + e->replacementLength(), QTextCursor::KeepAnchor);
		c.insertText(e->commitString());
	}

	QTextBlock block = m_oCursor->block();
	QTextLayout *layout = block.layout();
	//layout->setPreeditArea(m_oCursor->position() - block.position(), e->preeditString());
	QList<QTextLayout::FormatRange> overrides;
	//preeditCursor = e->preeditString().length();
	//hideCursor = false;
	for (int i = 0; i < e->attributes().size(); ++i)
	{
		const QInputMethodEvent::Attribute &a = e->attributes().at(i);
		///*if (a.type == QInputMethodEvent::Cursor) {
		//	preeditCursor = a.start;
		//	//hideCursor = !a.length;
		//} else*/
		if (a.type == QInputMethodEvent::TextFormat)
		{
			QTextCharFormat f = qvariant_cast<QTextFormat>(a.value).toCharFormat();
			if (f.isValid())
			{
				QTextLayout::FormatRange o;
				o.start = a.start + m_oCursor->position() - block.position();
				o.length = a.length;
				o.format = f;
				overrides.append(o);
			}
		}
	}
	layout->setAdditionalFormats(overrides);
	m_oCursor->endEditBlock();

	adjustSize();
	update();
}

