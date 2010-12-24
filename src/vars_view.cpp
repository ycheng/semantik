// Thomas Nagy 2007-2011 GPLV3

#include <QtDebug>
#include <QTextEdit>
#include <QKeyEvent>
#include <QScrollBar>
#include <QCompleter>
#include <QTreeView>
#include <QHeaderView>
#include <QAction>
#include <QStandardItemModel>
#include "con.h"
#include "data_item.h"
#include "sem_model.h"
#include "vars_view.h"

vars_view::vars_view(QWidget *i_oParent, sem_model *i_oControl) : QTextEdit(i_oParent)
{
	m_oControl = i_oControl;
	/*vars_highlighter *l_o =*/ new vars_highlighter(document());
	connect(this, SIGNAL(textChanged()), this, SLOT(update_edit()));

	m_iId = NO_ITEM;
	m_oCompleter = NULL;
}

void vars_view::init_completer()
{
	m_oCompleterAll = new QCompleter(this);
	m_oCompleterItem = new QCompleter(this);
	QTreeView *l_oTreeView = NULL;

	QStandardItemModel *l_oModelItem = new QStandardItemModel(this);

	fillloc(l_oModelItem);
	l_oTreeView = new QTreeView;
	m_oCompleterItem->setPopup(l_oTreeView);
	m_oCompleterItem->setModel(l_oModelItem);
	m_oCompleterItem->setWidget(this);
	m_oCompleterItem->setCompletionMode(QCompleter::PopupCompletion);
	m_oCompleterItem->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
	m_oCompleterItem->setCaseSensitivity(Qt::CaseInsensitive);
	l_oTreeView->header()->hide();
	l_oTreeView->header()->setStretchLastSection(false);
	l_oTreeView->header()->setResizeMode(0, QHeaderView::ResizeToContents);
	l_oTreeView->header()->setResizeMode(1, QHeaderView::Stretch);
	l_oTreeView->setRootIsDecorated(false);
	QObject::connect(m_oCompleterItem, SIGNAL(activated(const QString&)), this, SLOT(do_complete(const QString&)));



	QStandardItemModel *l_oModelAll = new QStandardItemModel(this);
	fillglo(l_oModelAll)


	l_oTreeView = new QTreeView;
	m_oCompleterAll->setPopup(l_oTreeView);
	m_oCompleterAll->setModel(l_oModelAll);
	m_oCompleterAll->setWidget(this);
	m_oCompleterAll->setCompletionMode(QCompleter::PopupCompletion);
	m_oCompleterAll->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
	m_oCompleterAll->setCaseSensitivity(Qt::CaseInsensitive);
	l_oTreeView->header()->setStretchLastSection(false);
	l_oTreeView->header()->setResizeMode(0, QHeaderView::ResizeToContents);
	l_oTreeView->header()->setResizeMode(1, QHeaderView::Stretch);
	l_oTreeView->setRootIsDecorated(false);
	l_oTreeView->header()->hide();
	QObject::connect(m_oCompleterAll, SIGNAL(activated(const QString&)), this, SLOT(do_complete(const QString&)));


	// initialize
	m_oCompleter = m_oCompleterAll;

	m_oCompleteAction = new QAction(trUtf8("Code complete"), this);
	m_oCompleteAction->setShortcut(trUtf8("Ctrl+space"));
	connect(m_oCompleteAction, SIGNAL(triggered()), this, SLOT(try_complete()));
	addAction(m_oCompleteAction);

#if 0
        QPalette l_oPalette = palette();
        l_oPalette.setBrush(QPalette::Inactive, QPalette::Base, QColor(230, 230, 230));
        l_oPalette.setBrush(QPalette::Disabled, QPalette::Base, QColor(230, 230, 230));
        setPalette(l_oPalette);
#endif
}

void vars_view::synchro_doc(const hash_params&i_o)
{
	int l_iCmd = i_o[data_commande].toInt();
	switch (l_iCmd)
	{
		case cmd_select_item:
			{
				m_iId = i_o[data_id].toInt();
				if (m_iId > NO_ITEM)
				{
					m_oCompleter = m_oCompleterItem;
					data_item *l_oData = m_oControl->m_oItems.value(m_iId);
					setText(l_oData->m_sHints);
				}
				else
				{
					m_oCompleter = m_oCompleterAll;
					setText(m_oControl->m_sHints);
				}
			}
			break;
		default:
			break;
	}
}

void vars_view::update_edit()
{
	if (m_iId)
	{
		data_item *l_oData = m_oControl->m_oItems.value(m_iId);
		l_oData->m_sHints = toPlainText();
	}
	else
	{
		m_oControl->m_sHints = toPlainText();
	}
}

vars_highlighter::vars_highlighter(QTextDocument *p): QSyntaxHighlighter(p)
{
	class_rule l_oRule;

	QTextCharFormat l_oSingle;
	l_oSingle.setForeground(Qt::darkBlue);
	l_oRule.m_oPattern = QRegExp("#[^\n]*");
	l_oSingle.setFontItalic(true);
	l_oRule.m_oFormat = l_oSingle;
	m_oRules.append(l_oRule);

	QTextCharFormat l_oKeyword;
	l_oKeyword.setForeground(Qt::darkGreen);
	l_oKeyword.setFontWeight(QFont::Bold);
	l_oRule.m_oPattern = QRegExp("^\\w+(\\.\\w+)*");
	l_oRule.m_oFormat = l_oKeyword;
	m_oRules.append(l_oRule);

	//QTextCharFormat l_oBad;
	//l_oBad.setForeground(Qt::red);
	//l_oRule.m_oPattern = QRegExp("^(\\w+\\b[^=]).*$");
	//l_oRule.m_oFormat = l_oBad;
	//m_oRules.append(l_oRule);
}

void vars_highlighter::highlightBlock(const QString &i_sText)
{
	foreach (class_rule l_oRule, m_oRules)
	{
		QRegExp l_oExp(l_oRule.m_oPattern);
		int i = i_sText.indexOf(l_oExp);
		while (i >= 0)
		{
			int l_iLen = l_oExp.matchedLength();

			if (i_sText[i+l_iLen-1]==QChar('=')) l_iLen--;
			setFormat(i, l_iLen, l_oRule.m_oFormat);
			i = i_sText.indexOf(l_oExp, i + l_iLen);
		}
	}
}

void vars_view::do_complete(const QString& i_s)
{
	QTextCursor l_oCursor = textCursor();
	int i_i = i_s.length() - m_oCompleter->completionPrefix().length();
	if (m_oCompleter->completionPrefix().length()>0)
	{
		l_oCursor.movePosition(QTextCursor::Left);
		l_oCursor.movePosition(QTextCursor::EndOfWord);
	}
	else
	{
		l_oCursor.movePosition(QTextCursor::StartOfWord);
	}
	l_oCursor.insertText(i_s.right(i_i));
	setTextCursor(l_oCursor);
}

void vars_view::keyPressEvent(QKeyEvent *i_oEv)
{
	if (m_oCompleter->popup()->isVisible())
	{
		switch (i_oEv->key())
		{
			case Qt::Key_Enter:
			case Qt::Key_Return:
			case Qt::Key_Escape:
			case Qt::Key_Tab:
			case Qt::Key_Backtab:
				i_oEv->ignore();
				return; // let the completer do default behavior
			default:
				break;
		}
	}

	QTextEdit::keyPressEvent(i_oEv);

	bool l_oCtrlOrShift = i_oEv->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);
	if (l_oCtrlOrShift && i_oEv->text().isEmpty())
	{
		return;
	}

	static QString eow("~!@#$%^&*()_+{}|:\"<>?,./;'[]\\-="); // end of word
	//bool hasModifier = (e->modifiers() != Qt::NoModifier) && !l_oCtrlOrShift;

	QTextCursor l_oCursor = textCursor();
	l_oCursor.select(QTextCursor::WordUnderCursor);
	QString l_sPrefix = l_oCursor.selectedText();

	if (
		l_sPrefix.length() < 1 or
		i_oEv->text().isEmpty() or
		eow.contains(i_oEv->text().right(1)) or
		(i_oEv->modifiers() != Qt::NoModifier && !l_oCtrlOrShift)
	   )
	{
		m_oCompleter->popup()->hide();
		return;
	}
	try_complete();
}

void vars_view::try_complete()
{
	QTextCursor l_oCursor = textCursor();
	l_oCursor.select(QTextCursor::WordUnderCursor);
	QString l_sPrefix = l_oCursor.selectedText();

	int l_iSave = l_oCursor.position();
	l_oCursor.movePosition(QTextCursor::StartOfWord);
	int l_iWord = l_oCursor.position();
	l_oCursor.movePosition(QTextCursor::StartOfLine);
	bool l_b = l_oCursor.position() == l_iWord;
	l_oCursor.setPosition(l_iSave);

	// TODO message for saying "can not complete"
	if (!l_b) return;

	if (l_sPrefix != m_oCompleter->completionPrefix())
	{
		m_oCompleter->setCompletionPrefix(l_sPrefix);
		m_oCompleter->popup()->setCurrentIndex(m_oCompleter->completionModel()->index(0, 0));
	}
	QRect l_oRect = cursorRect();
	l_oRect.setWidth(width());
	//m_oCompleter->popup()->sizeHintForColumn(0) +
	//	m_oCompleter->popup()->sizeHintForColumn(1) +
	//	m_oCompleter->popup()->verticalScrollBar()->sizeHint().width() +
	//	20

	QTreeView *l_oTreeView = (QTreeView*) m_oCompleter->popup();
	l_oTreeView->resizeColumnToContents(0);
	l_oTreeView->resizeColumnToContents(1);
	m_oCompleter->complete(l_oRect);
}

#include "vars_view.moc"

