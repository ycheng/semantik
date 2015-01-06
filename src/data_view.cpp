// Thomas Nagy 2007-2015 GPLV3

#include <QtDebug>
#include <QLabel>
#include <QCoreApplication>
#include "data_item.h"
#include "sem_mediator.h"
#include "data_view.h"
#include "con.h" 
#include "sembind.h" 

data_view::data_view(QWidget *i_oParent, sem_mediator *i_oControl) : QStackedWidget(i_oParent)
{
	m_oMediator = i_oControl;

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
	data_item *l_oItem = *m_oMediator + id;
	int j = 0;
	switch (l_oItem->m_iDataType)
	{
		// text image table diagram
		case VIEW_DIAG: j++;
		case VIEW_TABLE: j++;
		case VIEW_IMG: j++;
		case VIEW_TEXT: j++;
		default:
			setCurrentIndex(j);
			//qDebug()<<"index is"<<j;
			break;
	}
}

#include "data_view.moc"

