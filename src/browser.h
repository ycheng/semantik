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
	private:
		QMenu * m_oMenu;
};

#endif

