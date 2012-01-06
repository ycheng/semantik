// Thomas Nagy 2007-2012 GPLV3

#include "browser.h"
#include <QtDebug>
#include <QMenu>
#include <QContextMenuEvent>

browser::browser(QWidget *w) : QWebView(w)
{
	setContextMenuPolicy(Qt::DefaultContextMenu);
	m_oMenu = NULL;
}

void browser::contextMenuEvent(QContextMenuEvent * event)
{
	if (!m_oMenu)
	{
		m_oMenu = new QMenu(this);
        m_oMenu->addAction(pageAction(QWebPage::Reload));

		QAction *toggle = new QAction(trUtf8("Toggle fullscreen"), this);
		connect(toggle, SIGNAL(triggered()), this, SLOT(toggleFullScreen()));
		m_oMenu->addAction(toggle);
	}
	m_oMenu->popup(event->globalPos());

}

void browser::toggleFullScreen()
{
	if (isFullScreen())
	{
		setWindowModality(Qt::NonModal);
		//setFullScreen(false);
		setWindowState(Qt::WindowNoState);
		setWindowFlags(Qt::Widget);
		show();
	}
	else
	{
		setWindowFlags(Qt::Window);
		//setFullScreen(true);
		setWindowState(Qt::WindowFullScreen);
		setWindowModality(Qt::ApplicationModal);
		show();
	}
}

#include "browser.moc"

