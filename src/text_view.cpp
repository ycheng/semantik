// Thomas Nagy 2007-2009 GPLV3

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
#include "data_control.h"
#include "text_view.h"
#include "semantik.h"

text_view::text_view(QWidget *i_oParent, data_control *i_oControl) : QWidget(i_oParent)
{
	QGridLayout *l_oLayout = new QGridLayout();
	setLayout(l_oLayout);
	m_oLineEdit = new QLineEdit(this);

	m_oControl = i_oControl;

	m_oEdit = new KTextEdit(this);
	m_oEdit->setCheckSpellingEnabled(true);
	l_oLayout->addWidget(m_oLineEdit, 0, 0);
	l_oLayout->addWidget(m_oEdit, 1, 0, 1, 4);
	l_oLayout->setMargin(0);
	setMinimumHeight(30);

	m_iId = NO_ITEM;

	/*QPalette l_oPalette = m_oLineEdit->palette();
	l_oPalette.setBrush(QPalette::Inactive, QPalette::Base, QColor(230, 230, 230));
	l_oPalette.setBrush(QPalette::Disabled, QPalette::Base, QColor(230, 230, 230));
	m_oLineEdit->setPalette(l_oPalette);
	m_oEdit->setPalette(l_oPalette);*/

	connect(m_oLineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(update_summary(const QString &)));
	connect(m_oEdit, SIGNAL(textChanged()), this, SLOT(update_edit()));
	connect(m_oEdit, SIGNAL(currentCharFormatChanged(const QTextCharFormat &)),
            this, SLOT(char_format_changed(const QTextCharFormat &)));

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

void text_view::synchro_doc(const hash_params& i_o)
{
	int l_iCmd = i_o[data_commande].toInt();
	switch (l_iCmd)
	{
		case cmd_add_item:
			m_iId = i_o[data_id].toInt();
			break;

		case cmd_select_item:
			{
				m_iId = i_o[data_id].toInt();
				bool l_bCond = !m_iId;
				if (l_bCond)
				{
					m_oEdit->clear();
					m_oLineEdit->clear();
				}
				else
				{
					data_item *l_oData = m_oControl->m_oItems.value(m_iId);
					m_oLineEdit->setText(l_oData->m_sSummary);
					m_oEdit->setHtml(l_oData->m_sText);
				}

				m_oEdit->setReadOnly(l_bCond);
				m_oLineEdit->setReadOnly(l_bCond);

				m_oLineEdit->setEnabled(!l_bCond);
				m_oEdit->setEnabled(!l_bCond);
				m_oEdit->setCheckSpellingEnabled(!l_bCond);

				m_oBoldAct->setEnabled(!l_bCond);
				m_oItalicAct->setEnabled(!l_bCond);
				m_oUnderLineAct->setEnabled(!l_bCond);
			}
			break;
		case cmd_update_item:
			{
				int l_oId = i_o[data_id].toInt();
				if (m_iId)
				{
					data_item *l_oData = m_oControl->m_oItems.value(m_iId);
					if (l_oId == m_iId)
					{
						m_oLineEdit->setText(l_oData->m_sSummary);
					}
				}
				m_iId = l_oId;
			}
			break;
		default:
			break;
	}
}

void text_view::update_summary(const QString & i_s)
{
	if (!m_iId) return;
	data_item *l_oData = m_oControl->m_oItems.value(m_iId);

	bool l_b = (l_oData->m_sSummary != i_s);
	l_oData->m_sSummary = i_s;
	if (l_b) m_oControl->update_item(m_iId, VIEW_TEXT);
}

void text_view::update_edit()
{
	if (!m_iId) return;
	data_item *l_oData = m_oControl->m_oItems.value(m_iId);
	l_oData->m_sText = m_oEdit->toHtml();
	l_oData->m_iTextLength = m_oEdit->toPlainText().length();
	// no need to update here
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



#include "text_view.moc"

