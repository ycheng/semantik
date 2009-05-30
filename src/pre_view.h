// Thomas Nagy 2007-2009 GPLV3

#ifndef PRE_VIEW_H
#define PRE_VIEW_H

#include <QStackedWidget>
#include  	"con.h"

class QLabel;
class QWebView;
class pre_view : public QStackedWidget
{
	Q_OBJECT
	public:
		pre_view(QWidget *i_oParent, data_control *i_oControl);
		data_control *m_oControl;

		QLabel *m_oLabel;
		QWebView *m_oBrowser;


	public slots:
		void synchro_doc(const hash_params&);
};

#endif

