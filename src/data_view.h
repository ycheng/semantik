// Thomas Nagy 2007-2012 GPLV3

#ifndef DATA_VIEW_H
#define DATA_VIEW_H

#include <QStackedWidget>
#include  	"con.h"

class QLabel;
class data_view : public QStackedWidget
{
	Q_OBJECT
	public:
		data_view(QWidget *i_oParent, sem_mediator *i_oControl);
		sem_mediator *m_oMediator;
		QLabel *m_oLabel;

	public slots:
		void notify_select(const QList<int>& unsel, const QList<int>& sel);
		void notify_datatype(int id);
};

#endif

