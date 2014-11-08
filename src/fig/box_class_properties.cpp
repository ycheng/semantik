// Thomas Nagy 2007-2014 GPLV3

#include <KMessageBox>
#include <QSpinBox>
#include <QComboBox>
#include <QTreeWidget>
#include <QTextEdit>
#include <QAction>
#include <QGridLayout>
#include <QLabel>
#include <QHeaderView>
#include <QDirModel>
#include <QCompleter>
#include <QLineEdit>
#include <QTextEdit>
#include <QRegExp>
#include <QTreeView>
#include <QStandardItem>
#include <QKeyEvent>
#include "mem_box.h"
#include "box_view.h"

#include <QtDebug>

#include "box_class_properties.h"


class_editor::class_editor(QWidget *i_oParent) : QTextEdit(i_oParent)
{
	/*class_highlighter *l_o =*/ new class_highlighter(document());

	m_oCompleter = NULL;
	startup = true;
}

void class_editor::init_completer()
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

class_highlighter::class_highlighter(QTextDocument *p): QSyntaxHighlighter(p)
{
	class_highlighter_rule l_oRule;

	QTextCharFormat l_oSingle;
	l_oSingle.setForeground(Qt::darkGreen);
	l_oRule.m_oPattern = QRegExp("^((static|abstract|static abstract|abstract static)\\s+)*class\\s+");
	l_oSingle.setFontWeight(QFont::Bold);
	l_oRule.m_oFormat = l_oSingle;
	m_oRules.append(l_oRule);

	QTextCharFormat l_oKeyword;
	l_oKeyword.setForeground(Qt::darkGreen);
	l_oKeyword.setFontWeight(QFont::Bold);
	l_oRule.m_oPattern = QRegExp("^(public|private|protected|package|derived|stereotype)(\\s+(static|abstract|static abstract|abstract static)*)");
	l_oRule.m_oFormat = l_oKeyword;
	m_oRules.append(l_oRule);
}

void class_highlighter::highlightBlock(const QString &i_sText)
{
	foreach (class_highlighter_rule l_oRule, m_oRules)
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

void class_editor::do_complete(const QString& i_s)
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

void class_editor::keyPressEvent(QKeyEvent *i_oEv)
{
	// FIXME
	QTextEdit::keyPressEvent(i_oEv);
	return;


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

void class_editor::try_complete()
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




box_class_properties::box_class_properties(QWidget *i_oParent, box_class *i_oClass):
	KDialog(i_oParent), m_oClass(i_oClass)
{
	QWidget *widget = new QWidget(this);
	setCaption(trUtf8("Class properties"));
        setButtons(KDialog::Ok | KDialog::Apply | KDialog::Cancel);
	enableButtonApply(false);
	m_oClass = i_oClass;

	QGridLayout *l_oGridLayout = new QGridLayout(widget);

	m_oClassDefinition = new class_editor(widget);
	l_oGridLayout->addWidget(m_oClassDefinition, 0, 0);
	m_oClassDefinition->setStyleSheet("font-family: \"Courier New\", Courier, monospace;");

        setMainWidget(widget);
        QSize size(700, 320);
        resize(size.expandedTo(minimumSizeHint()));

	connect(m_oClassDefinition, SIGNAL(textChanged()), this, SLOT(enable_apply()));

        connect(this, SIGNAL(applyClicked()), this, SLOT(apply()));
        connect(this, SIGNAL(okClicked()), this, SLOT(apply()));

	m_oClassDefinition->setFocus();
}

void box_class_properties::enable_apply(int) {
	enableButtonApply(true);
}

void box_class_properties::enable_apply() {
	enableButtonApply(true);
}

void box_class_properties::apply() {
	if (!isButtonEnabled(KDialog::Apply)) {
		return;
	}

	QStringList l_oTmp = m_oClassDefinition->toPlainText().split(notr("\n"));

	if (l_oTmp.size() < 1) {
		KMessageBox::sorry(this, trUtf8("No class name given"), trUtf8("Missing class name"));
		return;
	}

	QRegExp rx("(static|abstract)?\\s*(static|abstract)?\\s*class\\s+(\\w.*)");
	if (rx.indexIn(l_oTmp[0]) != -1) {
		mem_class *mem = new mem_class(m_oClass->m_oView->m_oMediator, m_oClass->m_oView->m_iId);
		mem->init(m_oClass->m_oBox);

		mem->m_oNewBox.m_sText = rx.cap(3);
		mem->m_oNewBox.m_bStatic = (notr("static") == rx.cap(1) || notr("static") == rx.cap(2));
		mem->m_oNewBox.m_bAbstract = (notr("abstract") == rx.cap(1) || notr("abstract") == rx.cap(2));
		mem->m_oNewBox.m_oMethods.clear();
		mem->m_oNewBox.m_oAttributes.clear();

		QRegExp rm("^(public|private|protected|package|derived)\\s*(static|abstract)?\\s*(static|abstract)?\\s*(\\w.*)");
		QRegExp rs("^stereotype\\s*(.*)");
		for (int i=1; i < l_oTmp.size() - 1; ++i) {
			QString l_s = l_oTmp[i];
			if (rm.indexIn(l_s) >= 0) {
				QString l_sData = rm.cap(4);
				bool l_bStatic = (notr("static") == rm.cap(3) || notr("static") == rm.cap(2));
				bool l_bAbstract = (notr("abstract") == rm.cap(3) || notr("abstract") == rm.cap(2));

				if (l_sData.contains(notr("(")))
				{
					data_box_method m;
					m.m_bStatic = l_bStatic;
					m.m_bAbstract = l_bAbstract;
					m.m_sText = rm.cap(4);
					m.m_oVisibility = visibility::toVisibility(rm.cap(1));
					mem->m_oNewBox.m_oMethods.push_back(m);
				}
				else
				{
					data_box_attribute a;
					a.m_bStatic = l_bStatic;
					a.m_sText = rm.cap(4);
					a.m_oVisibility = visibility::toVisibility(rm.cap(1));
					mem->m_oNewBox.m_oAttributes.push_back(a);
				}
			} else if (rs.indexIn(l_s) >= 0) {
				mem->m_oNewBox.m_sStereotype = rs.cap(1);
			}
		}
		mem->apply();
		enableButtonApply(false);
	} else {
		qDebug()<<"no match for"<<l_oTmp[0];
		return;
	}
}

#include "box_class_properties.moc"

