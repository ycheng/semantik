// Thomas Nagy 2012 GPLV3

#ifndef EXPORT_MAP_DIALOG_H
#define EXPORT_MAP_DIALOG_H

#include <KDialog>

class QSpinBox;
class QRadioButton;
class KUrlRequester;
class export_map_dialog : public KDialog
{
	Q_OBJECT
	public:
		export_map_dialog(QWidget *i_oParent);

		KUrlRequester *kurlrequester;
		QRadioButton *m_oWidthC;
		QSpinBox *m_oWidth;
		QRadioButton *m_oHeightC;
		QSpinBox *m_oHeight;

		int m_iBaseWidth;
		int m_iBaseHeight;

	public slots:
		void radio(bool);
		void widthChanged(int);
		void heightChanged(int);
};

#endif

