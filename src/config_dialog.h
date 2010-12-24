// Thomas Nagy 2007-2011 GPLV3

#ifndef CONFIG_DIALOG_H
#define CONFIG_DIALOG_H

#include <QDialog>
#include <con.h>

class QComboBox;
class QSpinBox;
class QTreeWidgetItem;
class QPushButton;
class config_dialog : public QDialog
{
	Q_OBJECT
	public:
		config_dialog(QWidget *i_oParent);
		QComboBox *m_oReorgType;
		QComboBox *m_oConnType;
		QSpinBox *m_oAutoSave;
		QPushButton *m_oColorWidget;
		QColor m_oColor;

	public slots:
		void select_color();
};

#endif

