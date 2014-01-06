// Thomas Nagy 2007-2014 GPLV3

#include <fstream>
#include <QCoreApplication>
#include <QtGui>
#include  <QX11Info>
#include <QTranslator>
#include "con.h"
#include <stdlib.h>
#include <iostream>

#include <KGlobal>

#include <X11/Xlib.h>

#include <KApplication>
#include <KAboutData>
#include <KCmdLineArgs>
#include <KLocale>
#include <KDebug>
#include <KGlobal>

#include "semantik_d_win.h"

static const char description[] = I18N_NOOP("A tool for editing and organizing Semantik diagrams");
static const char version[] = VERSION;

int grid_int(int x) {
	return GRID * (x / GRID);
}

int main(int i_iArgc, char **i_iArgv)
{
	int l_iSeed;
	std::ifstream l_oInfile("/dev/urandom", std::ios::binary | std::ios::in);
	l_oInfile.read(reinterpret_cast<char *>(&l_iSeed), sizeof(int));
	l_oInfile.close();
	srandom(l_iSeed);

	KCmdLineOptions options;
	options.add("+[url]", ki18n("A file to open on startup"));
	options.add("o <file>", ki18n("An output file for exporting the diagram"));

	KAboutData l_o("semantik-d", 0, ki18n("Semantik-d"), version, ki18n(description), KAboutData::License_GPL_V3, ki18n("(C) 2013 Thomas Nagy"), KLocalizedString());
	l_o.setBugAddress("http://code.google.com/p/semantik/issues/list");
	l_o.addAuthor(ki18n("Thomas Nagy"), KLocalizedString());

	KCmdLineArgs::init(i_iArgc, i_iArgv, &l_o);
	KCmdLineArgs::addCmdLineOptions(options);

	KApplication l_oApp;

	KGlobal::locale()->insertCatalog("libkdeedu");

	semantik_d_win *l_oMainWin = new semantik_d_win;

	const KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
	if (!args->getOption("o").isEmpty())
	{
		if (!args->url(0).isValid())
		{
			return 1;
		}
		else
		{
			KUrl url(args->getOption("o"));
			if (!url.isValid()) {
				return 2;
			}

			l_oMainWin->slot_recent(args->url(0));
			l_oMainWin->print_current(url);
			return 0;
		}
	} else {
		if (args->count())
		{
			for (int i = 0; i < args->count(); ++i)
			{
				if (!args->url(i).isValid())
				{
					continue;
				}
				l_oMainWin->slot_recent(args->url(i));
			}
		}
		else
		{
			l_oMainWin->slot_add_tab();
		}
		l_oMainWin->show();
	}
	return l_oApp.exec();
}

