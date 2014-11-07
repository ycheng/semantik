// Thomas Nagy 2007-2014 GPLV3

#include <QSpinBox>
#include <QComboBox>
#include <QTreeWidget>
#include <QTextEdit>
#include <QGridLayout>
#include <QLabel>
#include <QHeaderView>
#include <QDirModel>
#include <QCompleter>
#include <QLineEdit>
#include <QTextEdit>
#include "mem_box.h"
#include "box_view.h"

#include <QtDebug>

#include "box_class_properties.h"

box_class_properties::box_class_properties(QWidget *i_oParent, box_class *i_oClass):
	KDialog(i_oParent), m_oClass(i_oClass)
{
	QWidget *widget = new QWidget(this);
	setCaption(trUtf8("Class properties"));
        setButtons(KDialog::Ok | KDialog::Apply | KDialog::Cancel);
	enableButtonApply(false);
	m_oClass = i_oClass;

	QGridLayout *l_oGridLayout = new QGridLayout(widget);

	m_oClassDefinition = new QTextEdit(widget);
	l_oGridLayout->addWidget(m_oClassDefinition, 0, 0);
	m_oClassDefinition->setStyleSheet("font-family: \"Courier New\", Courier, monospace;");

        setMainWidget(widget);
        QSize size(700, 320);
        resize(size.expandedTo(minimumSizeHint()));

	connect(m_oClassDefinition, SIGNAL(textChanged(const QString&)), this, SLOT(enable_apply(const QString &)));

        connect(this, SIGNAL(applyClicked()), this, SLOT(apply()));
        connect(this, SIGNAL(okClicked()), this, SLOT(apply()));

	m_oClassDefinition->setFocus();
}

void box_class_properties::enable_apply(int) {
	enableButtonApply(true);
}

void box_class_properties::enable_apply(const QString&) {
	enableButtonApply(true);
}

void box_class_properties::apply() {
	if (!isButtonEnabled(KDialog::Apply)) {
		return;
	}

	mem_class *mem = new mem_class(m_oClass->m_oView->m_oMediator, m_oClass->m_oView->m_iId);
	mem->init(m_oClass->m_oBox);
	//mem->m_sNewClassName = m_oClassName->text();
	mem->apply();
	enableButtonApply(false);
}

#include "box_class_properties.moc"

