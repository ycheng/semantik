// Thomas Nagy 2007-2012 GPLV3

#include <khtml_part.h>
#include <khtmlview.h>
#include <KUrl>
#include <QtDebug>
#include <QLabel>
#include <QTextBrowser>
#include <QCoreApplication>
#include <QWebView>
#include "data_item.h"
#include "sem_mediator.h"
#include "pre_view.h"
#include "con.h" 
#include "sembind.h" 
#include "browser.h"

pre_view::pre_view(QWidget *i_oParent, sem_mediator *i_oControl) : QStackedWidget(i_oParent)
{
	m_oMediator = i_oControl;

	m_oLabel = new QLabel(this);
	m_oLabel->setText(trUtf8("<p align='center'>No preview available<br/>at this time</p>"));
	addWidget(m_oLabel);

	/*m_oBrowserObject = new KHTMLPart(this, this);
	  m_oBrowser = m_oBrowserObject->widget(); //new QTextBrowser(this);
	  addWidget(m_oBrowser);*/
	m_oBrowser = new browser(this);
	addWidget(m_oBrowser);
}

void pre_view::notify_preview()
{
	QString l_s = bind_node::get_var(notr("preview"));
	if (l_s.endsWith(notr(".html")))
	{
		if (m_oBrowser->url() == QUrl(l_s))
		{
			m_oBrowser->reload();
		}
		else
		{
			m_oBrowser->load(QUrl(l_s));
			m_oBrowser->show();
		}
		setCurrentWidget(m_oBrowser);
	}
	else if (l_s.endsWith(notr(".tex")))
	{
		//m_oBrowser->openUrl(KUrl(l_s));
		//setCurrentWidget(m_oBrowser);
	}
	else
	{
		setCurrentWidget(m_oLabel);
	}

}

#include "pre_view.moc"

