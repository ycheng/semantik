// Thomas Nagy 2007-2011 GPLV3

#include <QAction>
#include <QtDebug>
#include <KTextEdit>
#include <QGridLayout>
#include <QLineEdit>
#include <QPalette>
#include <QX11Info>
#include<QCoreApplication>
#include <QToolButton>

#include <sonnet/speller.h>

//#include "aux.h"
//#include "con.h"
#include "data_item.h"
#include "sem_model.h"
#include "text_view.h"
#include "semantik.h"

text_view::text_view(QWidget *i_oParent, sem_model *i_oControl) : QWidget(i_oParent)
{
	QGridLayout *l_oLayout = new QGridLayout();
	setLayout(l_oLayout);
	//m_oLineEdit = new QLineEdit(this);

	m_oControl = i_oControl;

	m_oEdit = new KTextEdit(this);
	m_oEdit->setCheckSpellingEnabled(true);
	//l_oLayout->addWidget(m_oLineEdit, 0, 0);
	l_oLayout->addWidget(m_oEdit, 1, 0, 1, 4);
	l_oLayout->setMargin(0);
	setMinimumHeight(30);

	m_iId = NO_ITEM;

	/*QPalette l_oPalette = m_oLineEdit->palette();
	l_oPalette.setBrush(QPalette::Inactive, QPalette::Base, QColor(230, 230, 230));
	l_oPalette.setBrush(QPalette::Disabled, QPalette::Base, QColor(230, 230, 230));
	m_oLineEdit->setPalette(l_oPalette);
	m_oEdit->setPalette(l_oPalette);*/

	//connect(m_oLineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(update_summary(const QString &)));
	connect(m_oEdit, SIGNAL(textChanged()), this, SLOT(update_edit()));
	connect(m_oEdit, SIGNAL(currentCharFormatChanged(const QTextCharFormat &)), this, SLOT(char_format_changed(const QTextCharFormat &)));

	m_oBoldAct = new QAction(fetch_icon(notr("text_bold")), trUtf8("&Bold"), this);
	m_oBoldAct->setShortcut(trUtf8("Ctrl+B"));
	m_oBoldAct->setCheckable(true);

	m_oItalicAct = new QAction(fetch_icon(notr("text_italic")), trUtf8("&Italic"), this);
	m_oItalicAct->setShortcut(trUtf8("Ctrl+I"));
	m_oItalicAct->setCheckable(true);

	m_oUnderLineAct = new QAction(fetch_icon(notr("text_under")), trUtf8("&Underline"), this);
	m_oUnderLineAct->setShortcut(trUtf8("Ctrl+U"));
	m_oUnderLineAct->setCheckable(true);

	QToolButton *l_o = NULL;
	l_o = new QToolButton(this);
	l_o->setDefaultAction(m_oBoldAct);
	l_oLayout->addWidget(l_o, 0, 1);

	l_o = new QToolButton(this);
	l_o->setDefaultAction(m_oItalicAct);
	l_oLayout->addWidget(l_o, 0, 2);

	l_o = new QToolButton(this);
	l_o->setDefaultAction(m_oUnderLineAct);
	l_oLayout->addWidget(l_o, 0, 3);

	connect(m_oBoldAct, SIGNAL(triggered()), this, SLOT(text_bold()));
	connect(m_oItalicAct, SIGNAL(triggered()), this, SLOT(text_italic()));
	connect(m_oUnderLineAct, SIGNAL(triggered()), this, SLOT(text_underLine()));
}

void text_view::update_edit()
{
	if (!m_iId) return;
	data_item *l_oData = m_oControl->m_oItems.value(m_iId);

	mem_text* tmp = NULL;

	mem_command *c = NULL;
	if (!m_oControl->m_oUndoStack.empty())
	{
		c = m_oControl->m_oUndoStack.pop();
		m_oControl->m_oUndoStack.push(c);
		if (c->type() != mem_command::TEXT)
		{
			c = NULL;
		}
	}

	tmp = (mem_text*) c;
	if (!c) {
		tmp = new mem_text(m_oControl);
		tmp->sel = l_oData;
		tmp->oldText = l_oData->m_sText;
		tmp->add();
	}
	tmp->newText = tmp->sel->m_sText = m_oEdit->toHtml();
	tmp->sel->m_iTextLength = m_oEdit->toPlainText().length();
}

void text_view::text_bold()
{
	QTextCharFormat i_oFormat;
	i_oFormat.setFontWeight(m_oBoldAct->isChecked() ? QFont::Bold : QFont::Normal);
	merge_format(i_oFormat);
	update_edit();
}

void text_view::text_underLine()
{
	QTextCharFormat i_oFormat;
	i_oFormat.setFontUnderline(m_oUnderLineAct->isChecked());
	merge_format(i_oFormat);
	update_edit();
}

void text_view::text_italic()
{
	QTextCharFormat i_oFormat;
	i_oFormat.setFontItalic(m_oItalicAct->isChecked());
	merge_format(i_oFormat);
	update_edit();
}

void text_view::merge_format(const QTextCharFormat &i_oFormat)
{
	QTextCursor cursor = m_oEdit->textCursor();
	if (!cursor.hasSelection()) cursor.select(QTextCursor::WordUnderCursor);
	cursor.mergeCharFormat(i_oFormat);
	m_oEdit->mergeCurrentCharFormat(i_oFormat);
}

void text_view::char_format_changed(const QTextCharFormat &i_oFormat)
{
	QFont l_oF = i_oFormat.font();
	if (!m_oWin) return;

	m_oBoldAct->setChecked(l_oF.bold());
	m_oItalicAct->setChecked(l_oF.italic());
	m_oUnderLineAct->setChecked(l_oF.underline());
}

void text_view::notify_text(int id) {
	if (id == m_iId) {
		m_iId = NO_ITEM; // do not trigger the signal changed
		data_item *sel = *m_oControl + id;
		m_oEdit->setHtml(sel->m_sText);
		m_iId = id;
	}
}

void text_view::notify_select(const QList<int>& unsel, const QList<int>& sel) {
	bool one = (sel.size() == 1);
	m_iId = NO_ITEM;

	m_oEdit->setReadOnly(!one);
	m_oEdit->setEnabled(one);
	m_oEdit->setCheckSpellingEnabled(one);

	m_oBoldAct->setEnabled(one);
	m_oItalicAct->setEnabled(one);
	m_oUnderLineAct->setEnabled(one);

	if (one) {
		data_item *l_oData = m_oControl->m_oItems.value(sel.at(0));
		m_oEdit->setHtml(l_oData->m_sText);
		m_iId = sel.at(0);
	} else {
		m_oEdit->clear();
	}
}

#include "text_view.moc"

