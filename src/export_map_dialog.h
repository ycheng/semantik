// Thomas Nagy 2012 GPLV3

#ifndef EXPORT_MAP_DIALOG_H
#define EXPORT_MAP_DIALOG_H

#include <KDialog>

class QSpinBox;
class KUrlRequester;
class export_map_dialog : public QDialog
{
	Q_OBJECT
	public:
		export_map_dialog(QWidget *i_oParent);
		QSpinBox *m_oWidth;
		QSpinBox *m_oHeight;
		KUrlRequester *m_oRequester;
};

#endif

