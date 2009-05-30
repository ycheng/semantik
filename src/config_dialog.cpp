// Thomas Nagy 2007-2009 GPLV3

#include <QVariant>
#include <QAction>
#include <QSpinBox>
#include <QApplication>
#include <QButtonGroup>
#include <QDialog>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSpacerItem>
#include <QTreeWidget>
#include <QComboBox>
#include <QHeaderView>
#include <QDirModel>
#include <QCompleter>
#include <QPushButton>
#include <QCoreApplication>
#include <QtDebug>
#include  <QColorDialog>

#include "config_dialog.h"

config_dialog::config_dialog(QWidget *i_oParent):
	QDialog(i_oParent)
{
	QGridLayout *l_oGridLayout = new QGridLayout(this);

	QLabel *l_sLabel;

	l_sLabel = new QLabel(this);
	l_sLabel->setText(trUtf8("Reorganization type"));
	l_oGridLayout->addWidget(l_sLabel, 0, 0, 1, 1);

	l_sLabel = new QLabel(this);
	l_sLabel->setText(trUtf8("Connection type"));
	l_oGridLayout->addWidget(l_sLabel, 1, 0, 1, 1);

	l_sLabel = new QLabel(this);
	l_sLabel->setText(trUtf8("Save document automatically"));
	l_oGridLayout->addWidget(l_sLabel, 2, 0, 1, 1);

	l_sLabel = new QLabel(this);
	l_sLabel->setText(trUtf8("Background color"));
	l_oGridLayout->addWidget(l_sLabel, 3, 0, 1, 1);


	m_oReorgType = new QComboBox(this);
	m_oReorgType->addItems(QStringList()<<trUtf8("Fixed")<<trUtf8("Force-based")<<trUtf8("Incremental"));
	l_oGridLayout->addWidget(m_oReorgType, 0, 1, 1, 1);

	m_oConnType = new QComboBox(this);
	m_oConnType->addItems(QStringList()<<trUtf8("Lines")<<trUtf8("Splines"));
	l_oGridLayout->addWidget(m_oConnType, 1, 1, 1, 1);

	m_oAutoSave = new QSpinBox(this);
	m_oAutoSave->setSuffix(trUtf8(" min"));
	m_oAutoSave->setRange(0, 69 /* 69 hmmm */);
	l_oGridLayout->addWidget(m_oAutoSave, 2, 1, 1, 1);
	m_oAutoSave->setToolTip(trUtf8("Interval in minutes for saving the document automatically, 0 for disabling this feature"));

	m_oColorWidget = new QPushButton();
	l_oGridLayout->addWidget(m_oColorWidget, 3, 1, 1, 1);
	connect(m_oColorWidget, SIGNAL(clicked()), this, SLOT(select_color()));

	QSpacerItem *spacerItem = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
	l_oGridLayout->addItem(spacerItem, 4, 1, 1, 1);


	QDialogButtonBox *l_oButtonBox = new QDialogButtonBox(this);
	l_oButtonBox->setOrientation(Qt::Horizontal);
	l_oButtonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::NoButton|QDialogButtonBox::Ok);

	l_oGridLayout->addWidget(l_oButtonBox, 5, 0, 1, 2);

	connect(l_oButtonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(l_oButtonBox, SIGNAL(rejected()), this, SLOT(reject()));

	QSize size(321, 120);
	size = size.expandedTo(minimumSizeHint());
	resize(size);
}

void config_dialog::select_color()
{
	m_oColor = QColorDialog::getColor(m_oColor, this);
	if (!m_oColor.isValid()) return;
	m_oColorWidget->setText(m_oColor.name());

	QPalette l_oPalette = m_oColorWidget->palette();
	l_oPalette.setBrush(QPalette::Active, QPalette::Button, m_oColor);
	m_oColorWidget->setPalette(l_oPalette);
}

#include "config_dialog.moc"
