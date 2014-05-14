// Thomas Nagy 2014 GPLV3

#ifndef MATRIX_DIALOG_H
#define MATRIX_DIALOG_H

#include <QDialog>
#include <QSpinBox>
#include <con.h>
#include"aux.h"

class matrix_dialog : public QDialog
{
	Q_OBJECT
	public:
		matrix_dialog(QWidget *i_oParent);
		QSpinBox *m_oRows;
		QSpinBox *m_oCols;
};

#endif

