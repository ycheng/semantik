// Thomas Nagy 2007-2015 GPLV3

#include <QVariant>
#include <QAction>
#include <QApplication>
#include <QButtonGroup>
#include <KDialog>
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

export_map_dialog::export_map_dialog(QWidget *i_oParent): KDialog(i_oParent)
{
	setButtons(KDialog::Ok | KDialog::Cancel);

	QWidget *widget = new QWidget(this);
	setCaption(trUtf8("Export the map"));
	setButtons(KDialog::Ok | KDialog::Cancel);

        QGridLayout * gridLayout = new QGridLayout(widget);

	QLabel *label = new QLabel(widget);
	label->setText(trUtf8("File to write"));
	gridLayout->addWidget(label, 0, 0);

        kurlrequester = new KUrlRequester(widget);
        gridLayout->addWidget(kurlrequester, 0, 1, 1, 1);

        m_oWidthC = new QRadioButton(trUtf8("Width"), widget);
        gridLayout->addWidget(m_oWidthC, 1, 0, 1, 1);

        m_oWidth = new QSpinBox(widget);
	m_oWidth->setMinimum(16);
	m_oWidth->setMaximum(20000);
        gridLayout->addWidget(m_oWidth, 1, 1, 1, 1);

        m_oHeightC = new QRadioButton(trUtf8("Height"), widget);
        gridLayout->addWidget(m_oHeightC, 2, 0, 1, 1);

        m_oHeight = new QSpinBox(widget);
	m_oHeight->setMinimum(16);
	m_oHeight->setMaximum(20000);
	m_oHeight->setEnabled(false);
        gridLayout->addWidget(m_oHeight, 2, 1, 1, 1);

	connect(m_oHeightC, SIGNAL(clicked(bool)), this, SLOT(radio(bool)));
	connect(m_oWidthC, SIGNAL(clicked(bool)), this, SLOT(radio(bool)));
	connect(m_oWidth, SIGNAL(valueChanged(int)), this, SLOT(widthChanged(int)));
	connect(m_oHeight, SIGNAL(valueChanged(int)), this, SLOT(heightChanged(int)));

	setMainWidget(widget);
	QSize size(600, 100);
	resize(size.expandedTo(minimumSizeHint()));

	kurlrequester->setFocus();
}

void export_map_dialog::slotButtonClicked(int button) {
	if (button == KDialog::Ok)
	{
		accept();
	}
	else
	{
		KDialog::slotButtonClicked(button);
	}
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

void export_map_dialog::widthChanged(int v)
{
	if (m_oHeightC->isChecked())
		return;
	m_oHeight->setValue((v * m_iBaseHeight) / m_iBaseWidth);
}

void export_map_dialog::heightChanged(int v)
{
	if (m_oWidthC->isChecked())
		return;
	m_oWidth->setValue((v * m_iBaseWidth) / m_iBaseHeight);
}

#include "export_map_dialog.moc"

