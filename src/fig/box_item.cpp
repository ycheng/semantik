// Thomas Nagy 2007-2009 GPLV3


#include <QApplication>
#include <QAbstractTextDocumentLayout>
#include <QTextDocument>
#include <QTextDocumentFragment>
#include <QAbstractTextDocumentLayout>
#include <QTextList>
#include <QClipboard>
#include <QPainter>
#include <QtDebug>
#include <QTextDocument>
#include "box_item.h"
#include "box_view.h"

box_item::box_item(box_view* i_oParent) : QGraphicsRectItem()
{
	m_oControl = i_oParent;
	m_oDoc = new QTextDocument();
	m_oCursor = new QTextCursor(m_oDoc);
	m_bEdit = false;

	QTextOption l_oOpt;
#if QT_VERSION >= 0x040290
	l_oOpt.setWrapMode(QTextOption::WordWrap);
#endif
	l_oOpt.setAlignment(Qt::AlignCenter);
	m_oDoc->setDefaultTextOption(l_oOpt);

	m_iId = i_oParent->next_id();

	setBrush(QColor(170, 170, 255));
	i_oParent->scene()->addItem(this);

	setZValue(64);
}

box_item::~box_item()
{

}

bool box_item::moveKey(QKeyEvent* i_oEv)
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


void box_item::keyPressEvent(QKeyEvent* i_oEv)
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
		//TODO
	}
	else if (i_oEv == QKeySequence::Cut)
	{
#if 0
		const QTextDocumentFragment fragment(*m_oCursor);
		QMimeData *data = new QTextEditMimeData(fragment);
		QApplication::clipboard()->setMimeData(data);
		m_oCursor->removeSelectedText();
#endif
	}
	else if (i_oEv == QKeySequence::Paste)
	{
		const QMimeData *source = QApplication::clipboard()->mimeData();
		if (source)
		{
			QString text = source->text();
			if (!text.isNull())
			{
				m_oCursor->insertFragment( QTextDocumentFragment::fromPlainText(text) );
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

	//FIXME adjustSize();
	update();

	//QGraphicsTextItem::keyPressEvent(i_oEv);
	//FIXME update_links();
}

void box_item::keyReleaseEvent(QKeyEvent* i_oEv)
{
	//ensureCursorVisible();
	//m_oCursor->movePosition(QTextCursor::End);

	if (i_oEv->key() == Qt::Key_Enter || i_oEv->key() == Qt::Key_Return)
	{
		return;
	}
	//QGraphicsTextItem::keyReleaseEvent(i_oEv);
}

void box_item::focus_in()
{
	if (m_oDoc->toPlainText() == QObject::trUtf8("Empty")) m_oDoc->setPlainText("");
	m_bEdit = true;
	//FIXME m_oColorBackup = m_oColor;
	//FIXME m_oColor = s_oColorSelect;
	//QKeyEvent l_oKeyEvent = QKeyEvent(QEvent::KeyPress, Qt::Key_End, Qt::NoModifier);
	//keyPressEvent(&l_oKeyEvent);
	update();
}

void box_item::focus_out(QFocusEvent *i_oEv)
{
	if (!m_bEdit) return;

	// FIXME m_oColor = m_oColorBackup;
	m_bEdit = false;

	if (m_oDoc->toPlainText().isEmpty()) m_oDoc->setPlainText(QObject::trUtf8("Empty"));
	//FIXME m_oGraph->m_oControl->m_oItems.value(Id())->m_sSummary = m_oDoc->toPlainText();
	//FIXME m_oGraph->m_oControl->update_item(Id(), view_canvas);

	//QGraphicsTextItem::focusOutEvent(i_oEv);
	//FIXME adjustSize();
	//FIXME update_links();
	update();
}

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
		i_oPainter->drawEllipse(QRectF(l_o + l_oOffset, l_o - l_oOffset));
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
}

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

void box_item::inputMethodEvent(QInputMethodEvent *e)
{
	// TODO this is copied from qt
	if (!m_bEdit)
	{
		e->ignore();
		return;
	}
	m_oCursor->beginEditBlock();
	m_oCursor->removeSelectedText();

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

	//adjustSize();
	update();
}

