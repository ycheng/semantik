// Thomas Nagy 2007-2014 GPLV3

#ifndef LINEAR_CONTAINER_H
#define LINEAR_CONTAINER_H

#include <QWidget>

#include "con.h"

class QWidget;
class sem_mediator;
class linear_view;
class linear_container : public QWidget
{
	public:
		linear_container(QWidget *i_oParent, sem_mediator *i_oControl);

		sem_mediator *m_oMediator;
		linear_view *m_oView;
};

#endif

