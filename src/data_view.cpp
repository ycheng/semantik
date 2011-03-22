// Thomas Nagy 2007-2011 GPLV3

#include <QtDebug>
#include <QLabel>
#include <QCoreApplication>
#include "data_item.h"
#include "sem_model.h"
#include "data_view.h"
#include "con.h" 
#include "sembind.h" 

data_view::data_view(QWidget *i_oParent, sem_model *i_oControl) : QStackedWidget(i_oParent)
{
	m_oControl = i_oControl;

	m_oLabel = new QLabel(this);
	m_oLabel->setText(trUtf8("<p align='center'>No item selected</p>"));
	addWidget(m_oLabel);
}

void data_view::notify_select(const QList<int>& unsel, const QList<int>& sel) {
	if (sel.size() != 1) {
		setCurrentIndex(0);
	}
	else
	{
		notify_datatype(sel.at(0));
	}
}

void data_view::notify_datatype(int id) {
	data_item *l_oItem = *m_oControl + id;
	int j = 0;
	switch (l_oItem->m_iDataType)
	{
		// just to make it really complicated
		// text image table diagram
		case VIEW_DIAG: j++;
		case VIEW_TABLE: j++;
		case VIEW_IMG: j++;
		case VIEW_TEXT: j++;
		default:
			setCurrentIndex(j);
			break;
	}
}

#include "data_view.moc"

