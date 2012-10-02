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
#include <QRadioButton>
#include <kurlrequester.h>

#include <QtDebug>

#include "export_map_dialog.h"

export_map_dialog::export_map_dialog(QWidget *i_oParent): QWidget(i_oParent)
{
        QGridLayout * gridLayout = new QGridLayout(this);

	QLabel *label = new QLabel(this);
	label->setText(trUtf8("File to write"));
	gridLayout->addWidget(label, 0, 0);

        kurlrequester = new KUrlRequester(this);
        gridLayout->addWidget(kurlrequester, 0, 1, 1, 1);

        m_oWidthC = new QRadioButton(trUtf8("Width"), this);
        gridLayout->addWidget(m_oWidthC, 1, 0, 1, 1);

        m_oWidth = new QSpinBox(this);
	m_oWidth->setMinimum(16);
	m_oWidth->setMaximum(20000);
        gridLayout->addWidget(m_oWidth, 1, 1, 1, 1);

        m_oHeightC = new QRadioButton(trUtf8("Height"), this);
        gridLayout->addWidget(m_oHeightC, 2, 0, 1, 1);

        m_oHeight = new QSpinBox(this);
	m_oHeight->setMinimum(16);
	m_oHeight->setMaximum(20000);
	m_oHeight->setEnabled(false);
        gridLayout->addWidget(m_oHeight, 2, 1, 1, 1);

	connect(m_oHeightC, SIGNAL(clicked(bool)), this, SLOT(radio(bool)));
	connect(m_oWidthC, SIGNAL(clicked(bool)), this, SLOT(radio(bool)));
}

void export_map_dialog::radio(bool)
{
	if (m_oHeightC->isChecked())
	{
		m_oHeight->setEnabled(true);
		m_oWidth->setEnabled(false);
	}
	else
	{
		m_oWidth->setEnabled(true);
		m_oHeight->setEnabled(false);
	}
}

#include "export_map_dialog.moc"

