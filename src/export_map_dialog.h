// Thomas Nagy 2012 GPLV3

#ifndef EXPORT_MAP_DIALOG_H
#define EXPORT_MAP_DIALOG_H

#include <KDialog>

class QSpinBox;
class QCheckBox;
class KUrlRequester;
class export_map_dialog : public QDialog
{
	Q_OBJECT
	public:
		export_map_dialog(QWidget *i_oParent);

		KUrlRequester *kurlrequester;
		QCheckBox *m_oWidthC;
		QSpinBox *m_oWidth;
		QCheckBox *m_oHeightC;
		QSpinBox *m_oHeight;
		QCheckBox *m_oRemember;
};

#endif

