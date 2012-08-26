// Thomas Nagy 2007-2012 GPLV3

#include <QSpinBox>
#include <QComboBox>
#include <QTreeWidget>
#include <QDialogButtonBox>
#include <QTextEdit>
#include <QGridLayout>
#include <QLabel>
#include <QHeaderView>
#include <QDirModel>
#include <QCompleter>

#include <QtDebug>

#include "box_link_properties.h"

box_link_properties::box_link_properties(QWidget *i_oParent):
	QDialog(i_oParent)
{
	QGridLayout *l_oGridLayout = new QGridLayout(this);

	QLabel *l_sLabel;

	l_sLabel = new QLabel(this);
	l_sLabel->setText(trUtf8("Thickness"));
	l_oGridLayout->addWidget(l_sLabel, 0, 0);

	l_sLabel = new QLabel(this);
	l_sLabel->setText(trUtf8("Line Style"));
	l_oGridLayout->addWidget(l_sLabel, 1, 0);

	l_sLabel = new QLabel(this);
	l_sLabel->setText(trUtf8("Left arrow"));
	l_oGridLayout->addWidget(l_sLabel, 2, 0);

	l_sLabel = new QLabel(this);
	l_sLabel->setText(trUtf8("Right arrow"));
	l_oGridLayout->addWidget(l_sLabel, 3, 0);


	m_oThickness = new QSpinBox(this);
	m_oThickness->setMinimum(1);
	m_oThickness->setMaximum(3);
	l_oGridLayout->addWidget(m_oThickness, 0, 1);

	m_oStyle = new QComboBox(this);
	m_oStyle->addItem(trUtf8("Solid Line"), Qt::SolidLine);
	m_oStyle->addItem(trUtf8("Dot Line"), Qt::DotLine);
	m_oStyle->addItem(trUtf8("Dash Line"), Qt::DashLine);
	l_oGridLayout->addWidget(m_oStyle, 1, 1);

	m_oLeftArrow = new QComboBox(this);
	m_oLeftArrow->addItem(trUtf8("No arrow"), 0);
	m_oLeftArrow->addItem(trUtf8("Triangle"), 0);
	l_oGridLayout->addWidget(m_oLeftArrow, 2, 1);

	m_oRightArrow = new QComboBox(this);
	m_oRightArrow->addItem(trUtf8("No arrow"), 0);
	m_oRightArrow->addItem(trUtf8("Triangle"), 0);
	l_oGridLayout->addWidget(m_oRightArrow, 3, 1);

	QDialogButtonBox *l_oButtonBox = new QDialogButtonBox(this);
	l_oButtonBox->setOrientation(Qt::Horizontal);
	l_oButtonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::NoButton|QDialogButtonBox::Ok);

	l_oGridLayout->addWidget(l_oButtonBox, 4, 0, 1, 2);

	connect(l_oButtonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(l_oButtonBox, SIGNAL(rejected()), this, SLOT(reject()));

	QSize size(321, 120);
	size = size.expandedTo(minimumSizeHint());
	resize(size);
}

#include "box_link_properties.moc"

