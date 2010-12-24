// Thomas Nagy 2007-2009 GPLV3

#include <khtml_part.h>
#include <khtmlview.h>
#include <KUrl>
#include <QtDebug>
#include <QLabel>
#include <QTextBrowser>
#include <QCoreApplication>
#include <QWebView>
#include "data_item.h"
#include "sem_model.h"
#include "pre_view.h"
#include "con.h" 
#include "sembind.h" 
#include "browser.h"

pre_view::pre_view(QWidget *i_oParent, sem_model *i_oControl) : QStackedWidget(i_oParent)
{
	m_oControl = i_oControl;

	m_oLabel = new QLabel(this);
	m_oLabel->setText(trUtf8("<p align='center'>No preview available<br/>at this time</p>"));
	addWidget(m_oLabel);

	/*m_oBrowserObject = new KHTMLPart(this, this);
	m_oBrowser = m_oBrowserObject->widget(); //new QTextBrowser(this);
	addWidget(m_oBrowser);*/
	m_oBrowser = new browser(this);
	addWidget(m_oBrowser);
}

void pre_view::synchro_doc(const hash_params&i_o)
{
	int l_iCmd = i_o[data_commande].toInt();
	switch (l_iCmd)
	{
		case cmd_pre_view:
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
			break;
		case cmd_open_map:
			setCurrentWidget(m_oLabel);
			break;
		default:
			break;
	}
}

#include "pre_view.moc"

