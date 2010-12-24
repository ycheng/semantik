// Thomas Nagy 2007-2009 GPLV3

#ifndef LINEAR_CONTAINER_H
#define LINEAR_CONTAINER_H

#include <QWidget>

#include "con.h"

class QWidget;
class sem_model;
class linear_view;
class linear_container : public QWidget
{
	Q_OBJECT
	public:
		linear_container(QWidget *i_oParent, sem_model *i_oControl);

		sem_model *m_oControl;
		linear_view *m_oView;

	public slots:
		void synchro_doc(const hash_params&);
};

#endif

