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
#include "mem_box.h"
#include "box_view.h"

#include <QtDebug>

#include "box_link_properties.h"

box_link_properties::box_link_properties(QWidget *i_oParent, box_link *i_oLink):
	KDialog(i_oParent)
{
	QWidget *widget = new QWidget(this);
	setCaption(trUtf8("Link properties"));
        setButtons(KDialog::Ok | KDialog::Apply | KDialog::Cancel);
	enableButtonApply(false);
	m_oLink = i_oLink;

	QGridLayout *l_oGridLayout = new QGridLayout(widget);

	QLabel *l_sLabel;

	l_sLabel = new QLabel(widget);
	l_sLabel->setText(trUtf8("Thickness"));
	l_oGridLayout->addWidget(l_sLabel, 0, 0);

	l_sLabel = new QLabel(widget);
	l_sLabel->setText(trUtf8("Line Type"));
	l_oGridLayout->addWidget(l_sLabel, 1, 0);

	l_sLabel = new QLabel(widget);
	l_sLabel->setText(trUtf8("Line Style"));
	l_oGridLayout->addWidget(l_sLabel, 2, 0);

	l_sLabel = new QLabel(widget);
	l_sLabel->setText(trUtf8("Origin arrow"));
	l_oGridLayout->addWidget(l_sLabel, 3, 0);

	l_sLabel = new QLabel(widget);
	l_sLabel->setText(trUtf8("Target arrow"));
	l_oGridLayout->addWidget(l_sLabel, 4, 0);


	m_oThickness = new QSpinBox(widget);
	m_oThickness->setMinimum(1);
	m_oThickness->setMaximum(3);
	l_oGridLayout->addWidget(m_oThickness, 0, 1);

	m_oType = new QComboBox(widget);
	m_oType->addItem(trUtf8("Zigzag"), 0);
	m_oType->addItem(trUtf8("Straight"), 0);
	l_oGridLayout->addWidget(m_oType, 1, 1);

	m_oStyle = new QComboBox(widget);
	m_oStyle->addItem(trUtf8("Invisible line"), (int) Qt::NoPen);
	m_oStyle->addItem(trUtf8("Solid Line"), (int) Qt::SolidLine);
	m_oStyle->addItem(trUtf8("Dash Line"),  (int) Qt::DashLine);
	m_oStyle->addItem(trUtf8("Dot Line"),   (int) Qt::DotLine);
	l_oGridLayout->addWidget(m_oStyle, 2, 1);

	m_oLeftArrow = new QComboBox(widget);
	m_oLeftArrow->addItem(trUtf8("No arrow"), 0);
	m_oLeftArrow->addItem(trUtf8("Triangle"), 0);
	m_oLeftArrow->addItem(trUtf8("Link"), 0);
	m_oLeftArrow->addItem(trUtf8("Inheritance"), 0);
	m_oLeftArrow->addItem(trUtf8("Aggregation"), 0);
	l_oGridLayout->addWidget(m_oLeftArrow, 3, 1);

	m_oRightArrow = new QComboBox(widget);
	m_oRightArrow->addItem(trUtf8("No arrow"), 0);
	m_oRightArrow->addItem(trUtf8("Triangle"), 0);
	m_oRightArrow->addItem(trUtf8("Link"), 0);
	m_oRightArrow->addItem(trUtf8("Inheritance"), 0);
	m_oRightArrow->addItem(trUtf8("Aggregation"), 0);
	l_oGridLayout->addWidget(m_oRightArrow, 4, 1);

	setMainWidget(widget);
	QSize size(350, 120);
	resize(size.expandedTo(minimumSizeHint()));

	connect(this, SIGNAL(applyClicked()), this, SLOT(apply()));
	connect(this, SIGNAL(okClicked()), this, SLOT(apply()));

	connect(m_oThickness, SIGNAL(valueChanged(int)), this, SLOT(enable_apply(int)));
	connect(m_oType, SIGNAL(currentIndexChanged(int)), this, SLOT(enable_apply(int)));
	connect(m_oStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(enable_apply(int)));
	connect(m_oLeftArrow, SIGNAL(currentIndexChanged(int)), this, SLOT(enable_apply(int)));
	connect(m_oRightArrow, SIGNAL(currentIndexChanged(int)), this, SLOT(enable_apply(int)));
}

void box_link_properties::enable_apply(int) {
	enableButtonApply(true);
}

void box_link_properties::apply() {
	if (!isButtonEnabled(KDialog::Apply)) {
		return;
	}

	mem_edit_link *mem = new mem_edit_link(m_oLink->m_oView->m_oMediator, m_oLink->m_oView->m_iId);
	mem->link = m_oLink->m_oLink;

	mem->prev.copy_from(m_oLink->m_oInnerLink);
	mem->next.copy_from(m_oLink->m_oInnerLink);
	mem->next.border_width = m_oThickness->value();
	mem->next.m_iLineType = (data_link::LineType) m_oType->currentIndex();
	mem->next.pen_style = (Qt::PenStyle) m_oStyle->currentIndex();
	mem->next.m_iLeftArrow = (data_link::Arrow) m_oLeftArrow->currentIndex();
	mem->next.m_iRightArrow = (data_link::Arrow) m_oRightArrow->currentIndex();

	mem->apply();
	enableButtonApply(false);
}

#include "box_link_properties.moc"

