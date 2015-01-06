// Thomas Nagy 2007-2015 GPLV3

#ifndef TABLE_DIALOG_H
#define TABLE_DIALOG_H

#include <QDialog>
#include <con.h>

class QSpinBox;
class QTextEdit;
class table_dialog : public QDialog
{
	Q_OBJECT
	public:
		table_dialog(QWidget *i_oParent);
		QSpinBox *m_oRows;
		QSpinBox *m_oCols;
};

#endif

