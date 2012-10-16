// Thomas Nagy 2012 GPLV3

#ifndef BOX_LINK_PROPERTIES_H
#define BOX_LINK_PROPERTIES_H

#include <KDialog>

class QSpinBox;
class QComboBox;
class box_link_properties : public KDialog
{
	Q_OBJECT
	public:
		box_link_properties(QWidget *i_oParent);
		QSpinBox *m_oThickness;
		QComboBox *m_oType;
		QComboBox *m_oStyle;
		QComboBox *m_oLeftArrow;
		QComboBox *m_oRightArrow;
};

#endif

