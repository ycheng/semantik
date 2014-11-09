// Thomas Nagy 2012 GPLV3

#ifndef BOX_DOCUMENT_PROPERTIES_H
#define BOX_DOCUMENT_PROPERTIES_H

#include <kpagedialog.h>
#include "box_view.h"

class KFontChooser;
class box_document_properties : public KPageDialog
{
	Q_OBJECT
	public:
		box_document_properties(box_view*);
		KFontChooser *m_oFontChooser;
		box_view *m_oView;

	public slots:
		void apply();
};

#endif

