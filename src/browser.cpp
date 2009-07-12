
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
	}
	m_oMenu->popup(event->globalPos());

}

#include "browser.moc"

