// Thomas Nagy 2012 GPLV3

#ifndef BOX_CLASS_PROPERTIES_H
#define BOX_CLASS_PROPERTIES_H

#include <KDialog>
#include "box_class.h"

class QComboBox;
class QLineEdit;
class box_class_properties : public KDialog
{
	Q_OBJECT
	public:
		box_class_properties(QWidget*, box_class*);
		box_class *m_oClass;
		QTextEdit *m_oClassDefinition;

	public slots:
		void apply();
		void enable_apply(int);
		void enable_apply();
};

#endif

