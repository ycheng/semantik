// Thomas Nagy 2007-2012 GPLV3

#include <QSpinBox>
#include <QComboBox>
#include <QTreeWidget>
#include <QTextEdit>
#include <QGridLayout>
#include <QLabel>
#include <QHeaderView>
#include <QDirModel>
#include <QCompleter>

#include <QtDebug>

#include "box_link_properties.h"

box_link_properties::box_link_properties(QWidget *i_oParent):
	KDialog(i_oParent)
{
	QWidget *widget = new QWidget(this);
	setCaption(trUtf8("Link properties"));
        setButtons(KDialog::Ok | KDialog::Cancel);

	QGridLayout *l_oGridLayout = new QGridLayout(widget);

	QLabel *l_sLabel;

	l_sLabel = new QLabel(widget);
	l_sLabel->setText(trUtf8("Thickness"));
	l_oGridLayout->addWidget(l_sLabel, 0, 0);

	l_sLabel = new QLabel(widget);
	l_sLabel->setText(trUtf8("Line Style"));
	l_oGridLayout->addWidget(l_sLabel, 1, 0);

	l_sLabel = new QLabel(widget);
	l_sLabel->setText(trUtf8("Left arrow"));
	l_oGridLayout->addWidget(l_sLabel, 2, 0);

	l_sLabel = new QLabel(widget);
	l_sLabel->setText(trUtf8("Right arrow"));
	l_oGridLayout->addWidget(l_sLabel, 3, 0);


	m_oThickness = new QSpinBox(widget);
	m_oThickness->setMinimum(1);
	m_oThickness->setMaximum(3);
	l_oGridLayout->addWidget(m_oThickness, 0, 1);

	m_oStyle = new QComboBox(widget);
	m_oStyle->addItem(trUtf8("Invisible line"), (int) Qt::NoPen);
	m_oStyle->addItem(trUtf8("Solid Line"), (int) Qt::SolidLine);
	m_oStyle->addItem(trUtf8("Dot Line"),   (int) Qt::DotLine);
	m_oStyle->addItem(trUtf8("Dash Line"),  (int) Qt::DashLine);
	l_oGridLayout->addWidget(m_oStyle, 1, 1);

	m_oLeftArrow = new QComboBox(widget);
	m_oLeftArrow->addItem(trUtf8("No arrow"), 0);
	m_oLeftArrow->addItem(trUtf8("Triangle"), 0);
	l_oGridLayout->addWidget(m_oLeftArrow, 2, 1);

	m_oRightArrow = new QComboBox(widget);
	m_oRightArrow->addItem(trUtf8("No arrow"), 0);
	m_oRightArrow->addItem(trUtf8("Triangle"), 0);
	l_oGridLayout->addWidget(m_oRightArrow, 3, 1);

	setMainWidget(widget);
	QSize size(350, 120);
	resize(size.expandedTo(minimumSizeHint()));
}

#include "box_link_properties.moc"

