// Thomas Nagy 2007-2009 GPLV3

#include <KIcon>
#include <QtDebug>
#include <QTreeView>
#include <QGridLayout>
#include <KPushButton>
#include <QLineEdit>
#include <QSizePolicy>

#include  "sem_model.h"
#include  "linear_container.h"
#include  "linear_view.h"
#include  "con.h"

linear_container::linear_container(QWidget *i_oParent, sem_model *i_oControl) : QWidget(i_oParent)
{
	m_oControl = i_oControl;
	QGridLayout *l_oLayout = new QGridLayout();
	m_oView = new linear_view(this, i_oControl);

	KPushButton *l_oButton = new KPushButton(this);
	l_oButton->setIcon(KIcon(notr("edit-clear-locationbar")));
	l_oButton->setIconSize(QSize(22, 22));
	l_oButton->setFlat(true);
	l_oButton->setFocusPolicy(Qt::NoFocus);
	l_oButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	l_oButton->setMaximumSize(QSize(27, 27));
	l_oButton->resize(25, 25);
	l_oButton->setAutoDefault(false);
	QLineEdit *l_oEdit = new QLineEdit(this);

	connect(l_oButton, SIGNAL(clicked(bool)), l_oEdit, SLOT(clear()));
	connect(l_oEdit, SIGNAL(textChanged(const QString&)), m_oView, SLOT(filter_slot(const QString&)));

	l_oLayout->addWidget(l_oButton, 0, 0);
	l_oLayout->addWidget(l_oEdit, 0, 1);
	l_oLayout->addWidget(m_oView, 1, 0, 1, 2);

	setLayout(l_oLayout);
}

void linear_container::synchro_doc(const hash_params& i_oParams)
{
	m_oView->synchro_doc(i_oParams);
}

#ifdef WAF
#include "linear_container.moc"
#endif


