// Thomas Nagy 2007-2011 GPLV3

#ifndef PRE_VIEW_H
#define PRE_VIEW_H

#include <QStackedWidget>
#include  	"con.h"

class browser;
class QLabel;
class QWebView;
class pre_view : public QStackedWidget
{
	Q_OBJECT
	public:
		pre_view(QWidget *i_oParent, sem_model *i_oControl);
		sem_model *m_oControl;

		QLabel *m_oLabel;
		browser *m_oBrowser;


	public slots:
		void notify_preview();
};

#endif

