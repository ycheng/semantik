// Thomas Nagy 2007-2009 GPLV3

#ifndef DATA_VIEW_H
#define DATA_VIEW_H

#include <QStackedWidget>
#include  	"con.h"

class QLabel;
class data_view : public QStackedWidget
{
	Q_OBJECT
	public:
		data_view(QWidget *i_oParent, sem_model *i_oControl);
		sem_model *m_oControl;
		QLabel *m_oLabel;

	public slots:
		void synchro_doc(const hash_params&);
};

#endif

