// Thomas Nagy 2007-2014 GPLV3

#include <fstream>
#include <QCoreApplication>
#include <QtGui>
#include  <QX11Info>
#include <QTranslator>
#include "con.h"
#include "semantik.h"
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

static const char description[] = I18N_NOOP("A mind-mapping tool for KDE\nAvailable exclusively for open systems such as OpenSuse, Gentoo or Fedora.");
static const char version[] = VERSION;

int grid_int(int x) {
	return GRID * (x / GRID);
}

int main(int i_iArgc, char **i_iArgv)
{
	KCmdLineOptions options;
	options.add("+[url]", ki18n("A file to open on startup"));
	options.add("o <file>", ki18n("An output file for printing the map"));

	KAboutData l_o("semantik", 0, ki18n("Semantik"), version, ki18n(description),
			KAboutData::License_GPL_V3, ki18n("(C) 2007-2012 Thomas Nagy"), KLocalizedString());
	l_o.setBugAddress("http://code.google.com/p/semantik/issues/list");
	l_o.addAuthor(ki18n("Thomas Nagy"), KLocalizedString());

	KCmdLineArgs::init(i_iArgc, i_iArgv, &l_o);
	KCmdLineArgs::addCmdLineOptions(options);

	KApplication l_oApp;

	KGlobal::locale()->insertCatalog("libkdeedu");

	semantik_win *l_oMainWin = new semantik_win;

	const KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
	if (!args->getOption("o").isEmpty()) {
		if (args->count()) {
			if (args->url(0).isEmpty()) {
				qDebug()<<"a file requires a url";
				return 1;
			} else {
				KUrl l_oUrl(args->getOption("o"));
				if (!l_oUrl.isValid()) return 2;

				l_oMainWin->slot_recent(args->url(0));
				QPair<int, int> p;
				return l_oMainWin->print_to_file(l_oUrl, p);
			}
		}
	} else {
		l_oMainWin->show();
		if (args->count() && !args->url(0).isEmpty()) {
			l_oMainWin->slot_recent(args->url(0));
		}
	}
	return l_oApp.exec();
}

