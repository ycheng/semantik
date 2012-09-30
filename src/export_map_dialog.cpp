// Thomas Nagy 2007-2012 GPLV3

#include <QVariant>
#include <QAction>
#include <QApplication>
#include <QButtonGroup>
#include <QDialog>
#include <QDialogButtonBox>
#include <QGridLayout>
#include<QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QSpacerItem>
#include<QCoreApplication>
#include <QTreeWidget>
#include <QTextEdit>
#include <QHeaderView>
#include <QDirModel>
#include <QSpinBox>
#include <kurlrequester.h>

#include <QtDebug>

#include "export_map_dialog.h"

export_map_dialog::export_map_dialog(QWidget *i_oParent):
	QDialog(i_oParent)
{
	QGridLayout *l_oGridLayout = new QGridLayout(this);

	QLabel *label = new QLabel(this);
	label->setText(trUtf8("Width"));
	l_oGridLayout->addWidget(label, 0, 0);

	m_oWidth = new QSpinBox(this);
	l_oGridLayout->addWidget(m_oWidth, 0, 1);

	label = new QLabel(this);
	label->setText(trUtf8("Height"));
	l_oGridLayout->addWidget(label, 1, 0);

	m_oHeight = new QSpinBox(this);
	l_oGridLayout->addWidget(m_oHeight, 1, 1);


	label = new QLabel(this);
	label->setText(trUtf8("Destination"));
	l_oGridLayout->addWidget(label, 2, 0);

	m_oRequester = new KUrlRequester(this);
	l_oGridLayout->addWidget(m_oRequester, 2, 1);



	QSpacerItem *spacerItem = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
	l_oGridLayout->addItem(spacerItem, 4, 4, 1, 1);

	QDialogButtonBox *l_oButtonBox = new QDialogButtonBox(this);
	l_oButtonBox->setOrientation(Qt::Horizontal);


	QPushButton *b = NULL;
	b = new QPushButton(trUtf8("&Ok"));
	b->setDefault(true);
	connect(b, SIGNAL(pressed()), this, SLOT(slot_accept()));
	l_oButtonBox->addButton(b, QDialogButtonBox::YesRole);
	b = new QPushButton(trUtf8("&Cancel"));
	connect(b, SIGNAL(clicked()), this, SLOT(reject()));
	l_oButtonBox->addButton(b, QDialogButtonBox::NoRole);

	l_oGridLayout->addWidget(l_oButtonBox, 5, 0, 1, 2);


	QSize size(250, 100);
	size = size.expandedTo(minimumSizeHint());
	resize(size);
}

#include "export_map_dialog.moc"

