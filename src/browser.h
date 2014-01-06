// Thomas Nagy 2007-2014 GPLV3

#include <QWebView>

#ifndef _BROWSER_H
#define _BROWSER_H

class QMenu;
class browser: public QWebView
{
	Q_OBJECT
	public:
		browser(QWidget*);
		void contextMenuEvent(QContextMenuEvent * event);
	public slots:
		void toggleFullScreen();
	private:
		QMenu * m_oMenu;
};

#endif

