#! /usr/bin/env python
# encoding: utf-8
# Thomas Nagy, 2007-2014 (ita)

APPNAME = 'semantik'
VERSION = '0.9.2'

top = '.'

import os, sys, re, time
from waflib import Options, Logs, Configure, Errors, Utils

def compile_py(task):
	outfile = task.m_outputs[0].abspath()
	f = open(outfile, 'w')
	try:
		w = f.write
		w('<!DOCTYPE RCC><RCC version="1.0">\n<qresource>\n')
		for k in task.m_inputs:
			w(' <file>')
			w(k.m_name)
			w('</file>\n')
			w('</qresource>\n</RCC>')
	finally:
		f.close()

def build(bld):

	os.environ['LD_LIBRARY_PATH'] = ':'.join(bld.env['LIBPATH_KDECORE'] + [os.environ.get('LD_LIBRARY_PATH', '')])
	bld.targets = '*' # build all targets by default

	lst = 'src/fig/semd.cpp src/fig/semantik_d_win.cpp src/fig/diagram_document.cpp src/fig/filetree.cpp'

	tg = bld(
		features = 'cxx qt4 cxxshlib',
		source = bld.path.ant_glob('src/fig/*.cpp src/base/*.cpp', excl=lst),
		target = 'semantik',
		name = 'semlib',
		vnum = VERSION,
		use = 'QTCORE QTGUI QTWEBKIT QTXML PYEMBED QTSVG KDECORE KIO KDEUI KHTML',
		includes='. src src/fig src/base',
		install_path = '${KDE4_LIB_INSTALL_DIR}/')

	bld(features='cxx qt4 cxxprogram pyembed',
		source = bld.path.ant_glob('src/*.cpp'),
		use = 'QTCORE QTGUI QTWEBKIT QTXML QTSVG KDECORE KIO KDEUI KHTML semlib',
		target = 'src/semantik',
		install_path = '${KDE4_BIN_INSTALL_DIR}/',
		includes = '. src src/fig src/base')

	bld(features='cxx qt4 cxxprogram pyembed',
		source = lst,
		use = 'QTCORE QTGUI QTWEBKIT QTXML QTSVG KDECORE KIO KDEUI KHTML semlib',
		target = 'src/semantik-d',
		install_path = '${KDE4_BIN_INSTALL_DIR}/',
		includes = '. src src/fig src/base')

	bld.install_files('${SEMANTIK_DIR}', 'src/sembind.py')

	rt = 'src/templates/'

	glob = bld.path.ant_glob

	bld.install_files('${SEMANTIK_DIR}/flags', glob('src/flags/*.svg'))
	bld.install_files('${TEMPLATE_DIR}', glob(rt+'*.py'))

	bld.install_as('${TEMPLATE_DIR}/waf', 'waf', chmod=Utils.O755)

	bld.install_files('${TEMPLATE_DIR}/pdflatex', glob(rt+'pdflatex/*'))
	for x in 'html odt odp java'.split():
		bld.install_files('${TEMPLATE_DIR}/' + x, glob(rt+'%s/*' % x))

	for x in "color font inner outer theme".split():
		k = 'beamer/beamermindist/themes/'
		bld.install_files('${TEMPLATE_DIR}/' + k+x, bld.path.ant_glob(rt+k+x+'/*'))

	obj = bld(features='msgfmt', appname = 'semantik', langs=[x.path_from(bld.path).replace('.po', '') for x in bld.path.ant_glob('src/po/*.po')])

	bld.install_files('${TEMPLATE_DIR}/beamer/beamermindist/art/', glob(rt+'beamer/beamermindist/art/*'))
	bld.install_files('${TEMPLATE_DIR}/beamer/beamermindist/', glob(rt+'beamer/beamermindist/*'))
	bld.install_files('${TEMPLATE_DIR}/beamer/', rt+'beamer/main.tex')
	bld.install_files('${TEMPLATE_DIR}/beamer/', rt+'beamer/wscript')

	bld.install_files('${TEMPLATE_DIR}/s5', rt+'s5/index.html')
	bld.install_files('${TEMPLATE_DIR}/s5/ui/default/', glob(rt+'s5/ui/default/*'))
	bld.install_files('${SEMANTIK_DIR}/images', glob('src/images/*.svg'))
	bld.install_files('${FILTER_DIR}/', glob('src/filters/*'))

	bld.install_files('${KDE4_XDG_APPS_INSTALL_DIR}/', 'src/data/semantik.desktop src/data/semantik-d.desktop')
	bld.install_files('${MIME_DIR}/', 'src/data/semantik.xml')

	for x in ('', '-d'):
		bld.install_as('${KDE4_ICON_INSTALL_DIR}/oxygen/128x128/apps/semantik%s.png' % x, 'src/data/hi128-app-semantik%s.png'%x)
		bld.install_as('${KDE4_ICON_INSTALL_DIR}/oxygen/64x64/apps/semantik%s.png'% x, 'src/data/hi64-app-semantik%s.png'%x)
		bld.install_as('${KDE4_ICON_INSTALL_DIR}/oxygen/48x48/apps/semantik%s.png'% x, 'src/data/hi48-app-semantik%s.png'%x)
		bld.install_as('${KDE4_ICON_INSTALL_DIR}/oxygen/32x32/apps/semantik%s.png'% x, 'src/data/hi32-app-semantik%s.png'%x)
		bld.install_as('${KDE4_ICON_INSTALL_DIR}/oxygen/22x22/apps/semantik%s.png'% x, 'src/data/hi22-app-semantik%s.png'%x)

	bld.install_files('${KDE4_DATA_INSTALL_DIR}/semantik', 'src/data/semantikui.rc src/data/semantik-dui.rc src/data/tips')

	bld.add_post_fun(post_build)
	# update-mime-database /usr/share/mime ?

def configure(conf):
	def test(system):
		return (sys.platform.lower().rfind(system) > -1)

	err = "Semantik cannot work on %s, please install a Linux system from http://www.opensuse.org"
	if (test('linux')):
		Logs.pprint('GREEN', "You are using Linux, that's good (tm)")
	elif (test('bsd')):
		Logs.pprint('GREEN', "You are using a BSD system, that's good (tm)")
	elif (test('win32') or test('cygwin')):
		conf.fatal(err % "win32")
	elif (test('darwin')):
		conf.fatal(err % "osx")
	elif (test('solaris') or test('sunos')):
		conf.fatal(err % "Sun systems")

	kde_config = os.popen("which kde-config").read().strip()
	if kde_config:
		icons = os.popen('%s --path icon' % kde_config).read().strip()
	else:
		icons = '/usr/share/icons:/usr:/etc/opt/kde3/share/icons/:/opt/kde3/share/icons/:/var/lib/mandriva/kde-profiles/powerpackplus/share/icons/:/usr/share/icons/'
		if not Options.options.icons:
			Logs.warn("For now Semantik uses a few kde icons - if you have problems seeing the icons install kdebase")

	if Options.options.icons:
		icons = Options.options.icons

	conf.load('gcc g++ qt4')
	if not conf.env.CXX: conf.fatal('Semantik requires g++ (compilation only)')
	if not conf.env.QT_LRELEASE: conf.fatal('Semantik requires the program lrelease (from the Qt linguist package? - compilation only)')
	conf.load('python')
	if not conf.env.LIB_QTWEBKIT: conf.fatal('QtWebKit not found - Semantik requires Qt >= 4.4')
	if not conf.env.PYTHON: conf.fatal('Semantik requires Python >= 2.5 (development package for the compilation)')
	conf.check_python_version((2,4,2))
	conf.check_python_headers()
	if not conf.env.PYTHON_VERSION in "2.4 2.5 2.6 2.7".split(): Logs.warn('Bad Python version %s ' % str(conf.env.PYTHON_VERSION))
	#ret = ctx.cmd_output('python-config --cflags').strip().split()

	try:
		conf.load('kde4')
		Logs.pprint('GREEN', "And you even have kde4! we will use it")
	except Errors.ConfigurationError:
		raise
		#Logs.pprint('YELLOW', "kde4 is disabled (missing)")
		conf.fatal('Compiling Semantik requires kde4 -devel')

	conf.env.DEFINES = 'WAF'
	conf.define('ICONS', icons)
	conf.define('VERSION', VERSION)

	kdeconfig = conf.find_program('kde4-config')
	prefix = conf.cmd_and_log(Utils.to_list(kdeconfig) + ['--prefix']).strip()
	conf.env.SEMANTIK_DIR = conf.env.KDE4_DATA_INSTALL_DIR + '/semantik'
	conf.env.MIME_DIR = prefix + '/share/mime/packages'
	conf.define('SEMANTIK_DIR', conf.env.SEMANTIK_DIR)

	conf.env.TEMPLATE_DIR = conf.env.KDE4_DATA_INSTALL_DIR + '/semantik/templates/'
	conf.define('TEMPLATE_DIR', conf.env.TEMPLATE_DIR)
	conf.env.FILTER_DIR = conf.env.KDE4_DATA_INSTALL_DIR +'/semantik/filters/'
	conf.define('FILTER_DIR', conf.env.FILTER_DIR)
	conf.env.PICDIR = conf.env.KDE4_ICON_INSTALL_DIR #j('share/pixmaps/')
	conf.define('cmd_add_item', 0)
	conf.define('cmd_update_item', 1)
	conf.define('data_root', os.sep)
	conf.define('cmd_remove_item', 2)
	conf.define('cmd_link', 3)
	conf.define('cmd_unlink', 4)
	conf.define('data_outdir', 'tmp')
	conf.define('cmd_select_item', 5)
	conf.define('cmd_sort_item', 6)
	conf.define('cmd_save_data', 7)
	conf.define('cmd_update_colors', 8)
	conf.define('cmd_update_flags', 9)
	conf.define('cmd_open_map', 10)
	conf.define('cmd_pre_view', 11)
	conf.define('cmd_change_data', 12)
	conf.define('cmd_export_item', 13)

	lst = [x for x in conf.path.find_node('src/styles.txt').read().splitlines()]
	buf = []
	for x in lst:
		if not x: continue
		t = x.split('\t')
		buf.append("""xp(x,%d,"%s");yp(x,%d,trUtf8("%s"));""" % (len(buf), t[0], len(buf), t[1].replace('"', '\\"')))
	conf.define('fillglo(x)', ''.join(buf), quote=False)

	lst = [x for x in conf.path.find_node('src/styles_local.txt').read().splitlines()]
	buf = []
	for x in lst:
		if not x: continue
		t = x.split('\t')
		buf.append("""xp(x,%d,"%s");yp(x,%d,trUtf8("%s"));""" % (len(buf), t[0], len(buf), t[1].replace('"', '\\"')))
	conf.define('fillloc(x)', ''.join(buf), quote=False)


	conf.env.CXXFLAGS_PYEMBED = [x for x in conf.env.CXXFLAGS_PYEMBED if x != '-g']

	if not 'CXXFLAGS' in os.environ:
		conf.env.CXXFLAGS = '-O2 -pipe -Wall'.split()# -DDEBUG=1 -g'
		#conf.env.CXXFLAGS = ['-g']
	conf.write_config_header('aux.h')

	# the Debian packagers compile with --prefix=/usr and set /etc/ld.so.conf accordingly
	# the rpath is for end users installing into /usr/local/lib only
	sur = '/usr/local'
	if Options.options.prefix[:len(sur)] == sur:
		conf.env.RPATH_NABLAH = '/usr/local/lib'

	conf.define('GAP', 50, quote=False)

def options(opt):
	opt.load('kde4')
	opt.load('qt4')
	opt.load('python ')
	opt.add_option('--exe', action='store_true', default=False, help='execute semantik after the compilation (developers)')
	opt.add_option('--ddd', action='store_true', default=False, help='execute semantik-d after the compilation (developers)')
	opt.add_option('--icons', action='store', default='', help='icon dirs where to look for kde icons (configuration)')
	opt.add_option('--nomimes', action='store_true', default=False, help='do not run update-mime-database when installing')
	opt.add_option('--noldconfig', action='store_true', default=False, help='do not run lconfig when installing')

def post_build(bld):
	if bld.cmd == 'install':
		if not Options.options.noldconfig:
			try: bld.exec_command('/sbin/ldconfig 2> /dev/null')
			except Exception: pass
		if not Options.options.nomimes:
			try: bld.exec_command('update-mime-database %s' % os.path.split(bld.env.MIME_DIR)[0])
			except Exception: pass

	if Options.options.exe:
		#os.popen('export LD_LIBRARY_PATH=out/default/:$LD_LIBRARY_PATH; PATH=plugins:$PATH out/default/src/semantik')
		bld.exec_command('LD_LIBRARY_PATH=build/:$LD_LIBRARY_PATH build/src/semantik --style plastique', stdout=None, stderr=None)
	if Options.options.ddd:
		bld.exec_command('LD_LIBRARY_PATH=build/:$LD_LIBRARY_PATH build/src/semantik-d --style plastique', stdout=None, stderr=None)

