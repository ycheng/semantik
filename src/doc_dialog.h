// Thomas Nagy 2007-2014 GPLV3

#ifndef DOC_DIALOG_H
#define DOC_DIALOG_H

#include <QDialog>
#include <con.h>

class QLineEdit;
class QTextEdit;
class QTreeWidgetItem;
class doc_dialog : public QDialog
{
	Q_OBJECT
	public:
		doc_dialog(QWidget *i_oParent);
		QLineEdit *m_oFirstNameEdit;
		QLineEdit *m_oNameEdit;
		QLineEdit *m_oOrganizationEdit;
		QLineEdit *m_oEmailEdit;
		QString m_sCurrent;
};

#endif

