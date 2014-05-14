// Thomas Nagy 2014 GPLV3

#include <QSpinBox>
#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QSpacerItem>
#include <QTreeWidget>
#include <QTextEdit>
#include <QHeaderView>
#include <QDirModel>
#include <QCompleter>

#include <QtDebug>

#include "matrix_dialog.h"

matrix_dialog::matrix_dialog(QWidget *i_oParent):
	QDialog(i_oParent)
{
	QGridLayout *l_oGridLayout = new QGridLayout(this);

	QLabel *l_sLabel;

	l_sLabel = new QLabel(this);
	l_sLabel->setText(trUtf8("Rows"));
	l_oGridLayout->addWidget(l_sLabel, 0, 0, 1, 1);

	l_sLabel = new QLabel(this);
	l_sLabel->setText(trUtf8("Columns"));
	l_oGridLayout->addWidget(l_sLabel, 1, 0, 1, 1);


	m_oRows = new QSpinBox(this);
	m_oRows->setMinimum(1);
	m_oRows->setMaximum(100);
	l_oGridLayout->addWidget(m_oRows, 0, 1, 1, 1);

	m_oCols = new QSpinBox(this);
	m_oCols->setMinimum(1);
	m_oCols->setMaximum(100);
	l_oGridLayout->addWidget(m_oCols, 1, 1, 1, 1);


	QSpacerItem *spacerItem = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
	l_oGridLayout->addItem(spacerItem, 2, 1, 1, 1);


	QDialogButtonBox *l_oButtonBox = new QDialogButtonBox(this);
	l_oButtonBox->setOrientation(Qt::Horizontal);
	l_oButtonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::NoButton|QDialogButtonBox::Ok);

	l_oGridLayout->addWidget(l_oButtonBox, 3, 0, 1, 2);

	connect(l_oButtonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(l_oButtonBox, SIGNAL(rejected()), this, SLOT(reject()));

	QSize size(321, 120);
	size = size.expandedTo(minimumSizeHint());
	resize(size);
}

#include "matrix_dialog.moc"
