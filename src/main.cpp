// Thomas Nagy 2007-2012 GPLV3

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
#include <X11/extensions/Xrender.h>

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
	int l_iSeed;
	std::ifstream l_oInfile("/dev/urandom", std::ios::binary | std::ios::in);
	l_oInfile.read(reinterpret_cast<char *>(&l_iSeed), sizeof(int));
	l_oInfile.close();
	srandom(l_iSeed);

	KCmdLineOptions options;
	options.add("+[url]", ki18n("A file to open on startup"));

	KAboutData l_o("semantik", 0, ki18n("Semantik"), version, ki18n(description),
			KAboutData::License_GPL_V3, ki18n("(C) 2007-2012 Thomas Nagy"), KLocalizedString(),
			"tnagy@outlook.com");
	l_o.setBugAddress("http://code.google.com/p/semantik/issues/list");
	l_o.addAuthor(ki18n("Thomas Nagy"), KLocalizedString());

	KCmdLineArgs::init(i_iArgc, i_iArgv, &l_o);
	KCmdLineArgs::addCmdLineOptions(options);

	KApplication l_oApp;

	KGlobal::locale()->insertCatalog("libkdeedu");

	semantik_win *l_oMainWin = new semantik_win;
	l_oMainWin->show();

	const KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
	for (int i = 0; i < args->count(); ++i)
	{
		l_oMainWin->slot_recent(args->url(i));
	}

	return l_oApp.exec();
}

