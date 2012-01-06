// Thomas Nagy 2007-2012 GPLV3

#ifndef FLAG_SCHEME_H
#define FLAG_SCHEME_H

//#include <QIcon>
class QIcon;
//#include <QSvgRenderer>
class QSvgRenderer;
#include <QString>
#include <con.h>

class flag_scheme: public QObject
{
	Q_OBJECT
	public:
		flag_scheme(QObject *i_oParent);
		~flag_scheme();
		flag_scheme(QObject *i_oParent, QString i_sPath, QString i_sName);

		QString m_sId;
		QString m_sName;
		QString m_sIconPath;
		QSvgRenderer *m_oRenderer;

		QSvgRenderer* _renderer();

		const QIcon _icon();
};

#endif

