// Thomas Nagy 2007-2009 GPLV3

#ifndef COLOR_SCHEME_H
#define COLOR_SCHEME_H

#include <QColor>
#include "con.h"
#include <QString>

class color_scheme // :
{
	public:
		color_scheme();

		QString m_sName;
		QColor m_oInnerColor;
		QColor m_oBorderColor;
		QColor m_oTextColor;
};


#endif

