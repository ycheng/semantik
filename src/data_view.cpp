// Thomas Nagy 2007-2009 GPLV3

#include <QtDebug>
#include <QLabel>
#include <QCoreApplication>
#include "data_item.h"
#include "data_control.h"
#include "data_view.h"
#include "con.h" 
#include "sembind.h" 

data_view::data_view(QWidget *i_oParent, data_control *i_oControl) : QStackedWidget(i_oParent)
{
	m_oControl = i_oControl;

	m_oLabel = new QLabel(this);
	m_oLabel->setText(trUtf8("<p align='center'>No item selected</p>"));
	addWidget(m_oLabel);
}

void data_view::synchro_doc(const hash_params&i_o)
{
	int l_iCmd = i_o[data_commande].toInt();
	switch (l_iCmd)
	{
		case cmd_select_item:
			{
			int l_iId = i_o[data_id].toInt();
			if (l_iId == NO_ITEM)
			{
				setCurrentIndex(0);
				break;
			}
			}
			// else fall through
		case cmd_change_data:
			{
			data_item *l_oItem = *m_oControl + i_o[data_id].toInt();
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
			break;
			}
		default:
			break;
	}
}

#include "data_view.moc"

