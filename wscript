#! /usr/bin/env python
# encoding: utf-8
# Thomas Nagy, 2007-2009 (pvm)

APPNAME='semantik'
VERSION='0.7.4'

srcdir = '.'
blddir = 'out'

import os, sys, re
import Task, Utils, Configure, Options, Logs

try: import hashlib as md5
except: import md5

#import Configure
#Configure.autoconfig = 1

#Params.g_autoconfig=1

def dist():
	from Scripting import dist
	filename = dist(APPNAME, VERSION)
	f = file(filename,'rb')
	m = md5.md5()
	readBytes = 100000
	while (readBytes):
		readString = f.read(readBytes)
		m.update(readString)
		readBytes = len(readString)
	f.close()
	print filename, m.hexdigest()
	sys.exit(0)

def compile_py(task):
	outfile = task.m_outputs[0].abspath(task.m_env)
	f = open(outfile, 'w')
	w = f.write
	w('<!DOCTYPE RCC><RCC version="1.0">\n<qresource>\n')
	for k in task.m_inputs:
		w(' <file>')
		w(k.m_name)
		w('</file>\n')
		w('</qresource>\n</RCC>')
	f.close()

def build(bld):
	env = bld.env_of_name('default')

	Options.options.compile_targets = 'camlprog,util/ocamltwt,nablah,src/semantik'

	os.environ['LD_LIBRARY_PATH'] = env['LIBPATH_KDECORE']+':'+os.environ.get('LD_LIBRARY_PATH', '')

	import ocaml
	ocaml.open_re = re.compile('open ([a-zA-Z]+)', re.M)

	ibj = bld.new_task_gen('ocaml')
	ibj.type = 'native'
	ibj.includes = 'util'
	ibj.find_sources_in_dirs('util')
	ibj.target = 'util/ocamltwt'
	ibj.are_deps_set=1
	ibj.uselib='PP'

	bld.add_group()

	abj = bld.new_task_gen('ocaml')
	abj.type = 'c_object'
	abj.find_sources_in_dirs('src', exts=['.c', '.ml', '.mli'])
	abj.includes='. src'
	abj.target='camlprog'
	abj.are_deps_set = 1
	abj.uselib = 'TWT'

	'''
	ibj = bld.new_task_gen(features=['cxx', 'qt4', 'cshlib'], target = 'nablah', uselib='QTCORE QTGUI QTWEBKIT', includes='. src src/wp')
	ibj.find_sources_in_dirs('src/wp', exts=['.cpp'])
	ibj.install_path = '${KDE4_LIB_INSTALL_DIR}/'
	ibj.post()
	'''

	ibj = bld.new_task_gen(features = 'cxx qt4 cshlib')
	ibj.find_sources_in_dirs('src/wp', exts=['.cpp'])
	ibj.target = 'nablah'
	ibj.uselib = 'QTCORE QTGUI QTWEBKIT'
	ibj.includes='. src src/wp'
	ibj.install_path = '${KDE4_LIB_INSTALL_DIR}/'

	import time
	obj = bld.new_task_gen(features='cxx qt4 cprogram pyembed')
	obj.find_sources_in_dirs('src src/fig', excludes=['src/aux.c'], exts=['.cpp'])
	obj.uselib = 'QTCORE QTGUI QTXML QTSVG CAML NABLAH KDECORE KIO KDEUI KHTML'
	obj.uselib_local = 'nablah'
	obj.target = 'src/semantik'
	obj.install_path = '${KDE4_BIN_INSTALL_DIR}/'
	#obj.defines = ['''TT1=\'"%s"\'''' % time.ctime(), '''TT2='"%s"' ''' % time.ctime(), 'foo=2']

	obj.includes = '. src src/fig'
	obj.add_objects = 'camlprog'

	import Task
	Task.TaskBase.classes['ocalinkx'].before = 'cxx_link'

	bld.install_files('${SEMANTIK_DIR}', 'src/sembind.py')

	rt = 'src/templates/'

	bld.install_files('${SEMANTIK_DIR}/flags', 'src/flags/*.svg')
	bld.install_files('${TEMPLATE_DIR}', rt+'*.py')

	if Options.is_install:
		tgt = bld.env_of_name('default')['TEMPLATE_DIR']+'waf'
		if env.get_destdir(): tgt = os.path.join(env.get_destdir(), tgt.lstrip(os.sep))
		bld.do_install(os.path.abspath('waf'), tgt)

	bld.install_files('${TEMPLATE_DIR}/pdflatex', rt+'pdflatex/*')
	for x in 'html odt odp java'.split():
		bld.install_files('${TEMPLATE_DIR}/' + x, rt+'%s/*' % x)

	for x in "color font inner outer theme".split():
		k = 'beamer/beamermindist/themes/'
		bld.install_files('${TEMPLATE_DIR}/' + k+x, rt+k+x+'/*')

	obj = bld.new_task_gen('msgfmt')
	obj.appname = 'semantik'
	obj.langs=['src/po/'+x for x in 'fr es'.split()]

	bld.install_files('${TEMPLATE_DIR}/beamer/beamermindist/art/', rt+'beamer/beamermindist/art/*')
	bld.install_files('${TEMPLATE_DIR}/beamer/beamermindist/', rt+'beamer/beamermindist/*.???')
	bld.install_files('${TEMPLATE_DIR}/beamer/', rt+'beamer/main.tex')
	bld.install_files('${TEMPLATE_DIR}/beamer/', rt+'beamer/wscript')

	bld.install_files('${TEMPLATE_DIR}/s5', rt+'s5/index.html')
	bld.install_files('${TEMPLATE_DIR}/s5/ui/default/', rt+'s5/ui/default/*')
	bld.install_files('${SEMANTIK_DIR}/images', 'src/images/*.svg')
	bld.install_files('${FILTER_DIR}/', 'src/filters/*')

	bld.install_files('${KDE4_XDG_APPS_INSTALL_DIR}/', 'src/data/semantik.desktop')
	bld.install_as('${KDE4_ICON_INSTALL_DIR}/hicolor/128x128/apps/semantik.png', 'src/data/hi128-app-semantik.png')
	bld.install_as('${KDE4_ICON_INSTALL_DIR}/hicolor/64x64/apps/semantik.png', 'src/data/hi64-app-semantik.png')
	bld.install_as('${KDE4_ICON_INSTALL_DIR}/hicolor/48x48/apps/semantik.png', 'src/data/hi48-app-semantik.png')
	bld.install_as('${KDE4_ICON_INSTALL_DIR}/hicolor/22x22/apps/semantik.png', 'src/data/hi22-app-semantik.png')
	bld.install_files('${KDE4_DATA_INSTALL_DIR}/semantik', 'src/data/semantikui.rc src/data/tips')

	if Options.commands['install'] or Options.commands['uninstall']:
		lst = os.listdir('src/images')
		lst = [x for x in lst if (x.rfind('hi')>-1)]
		for x in lst:
			els = x.split('-')
			size = els[0][2:]

			uh = els[2].replace('.png', '')
			uh = 'oxygen/%sx%s/actions/%s.png' % (size, size, uh)
			bld.install_as('${KDE4_ICON_INSTALL_DIR}/' + uh, 'src/images/%s' % x)

	bld.add_post_fun(post_build)

def configure(conf):
	def test(system):
		return (sys.platform.lower().rfind(system) > -1)

	err = "Semantik cannot work on %s, please install a Linux system from http://www.opensuse.org"
	if (test('linux')):
		Utils.pprint('GREEN', "You are using Linux, that's good (tm)")
	elif (test('bsd')):
		Utils.pprint('GREEN', "You are using a BSD system, that's good (tm)")
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
			Logs.warn("For now Semantik uses a few kde icons - if you have problems seeing the icons install kde")

	if Options.options.icons:
		icons = Options.options.icons

	conf.check_tool('gcc g++ qt4')
	if not conf.env.CXX: conf.fatal('Semantik requires g++ (compilation only)')
	if not conf.env.QT_LRELEASE: conf.fatal('Semantik requires the program lrelease (from the Qt linguist package? - compilation only)')
	conf.check_tool('python')
	if conf.env.HAVE_QTWEBKIT != 1: conf.fatal('QtWebKit not found - Semantik requires Qt >= 4.4')
	if not conf.env.PYTHON: conf.fatal('Semantik requires Python >= 2.5 (development package for the compilation)')
	conf.check_python_version((2,4,2))
	conf.check_python_headers()
	if not conf.env.PYTHON_VERSION in "2.4 2.5 2.6 2.7".split(): Logs.warn('Bad Python version %s ' % str(conf.env.PYTHON_VERSION))
	conf.check_tool('ocaml')
	if not conf.env.OCAMLOPT: conf.fatal('Semantik requires ocamlopt (compilation only)')

	#ret = Utils.cmd_output('python-config --cflags').strip().split()
	#conf.env.append_value('CXXFLAGS_CAML', ret)

	try:
		conf.check_tool('kde4')
		Utils.pprint('GREEN', "And you even have kde4! we will use it")
	except Configure.ConfigurationError:
		raise
		#Utils.pprint('YELLOW', "kde4 is disabled (missing)")
		conf.fatal('Compiling Semantik requires kde4 -devel')

	conf.env.OCALINKFLAGS_OPT_PP = 'str.cmxa'
	conf.env.LINKFLAGS_CAML = '-lasmrun -lstr -lnums'.split()
	conf.env.LIBPATH_CAML = conf.env.OCAMLLIB
	conf.env.LIB_CAML = "m".split()
	conf.env.OCAMLFLAGS_TWT = '-pp default/util/ocamltwt'.split() #-unsafe -noassert -inline 10'
	conf.env.CXXDEFINES ='WAF'
	conf.define('ICONS', icons)
	conf.define('VERSION', VERSION)

	conf.define('SEMANTIK_DIR', conf.env.KDE4_DATA_INSTALL_DIR + '/semantik')
	conf.define('TEMPLATE_DIR', conf.env.KDE4_DATA_INSTALL_DIR + '/semantik/templates/')
	conf.define('FILTER_DIR', conf.env.KDE4_DATA_INSTALL_DIR +'/semantik/filters/')
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


	if Options.options.use64:
		conf.env.shlib_INST_DIR = 'lib64'

	conf.env.CXXFLAGS_PYEMBED = [x for x in conf.env.CXXFLAGS_PYEMBED if x != '-g']

	conf.env.CXXFLAGS = '-O2 -pipe -Wall'.split()# -DDEBUG=1 -g'
	conf.write_config_header('aux.h')

	# the Debian packagers compile with --prefix=/usr and set /etc/ld.so.conf accordingly
	# the rpath is for end users installing into /usr/local/lib only
	sur = '/usr/local'
	if Options.options.prefix[:len(sur)] == sur:
		conf.env.RPATH_NABLAH = '/usr/local/lib'

def set_options(opt):
	opt.tool_options('kde4')
	opt.tool_options('qt4')
	opt.tool_options('python')
	opt.add_option('--exe', action='store_true', default=False, help='execute after the compilation (developers)')
	opt.add_option('--icons', action='store', default='', help='icon dirs where to look for kde icons (configuration)')
	opt.add_option('--use64', action='store_true', default=False, help='set the installation into lib+64 (configuration)')

def post_build(bld):
	if Options.commands['install']:
		try: os.popen('/sbin/ldconfig 2> /dev/null')
		except: pass
	if Options.options.exe:
		#os.popen('export LD_LIBRARY_PATH=out/default/:$LD_LIBRARY_PATH; PATH=plugins:$PATH out/default/src/semantik')
		Utils.pproc.Popen('LD_LIBRARY_PATH=out/default/:$LD_LIBRARY_PATH out/default/src/semantik --style plastique', shell=True).wait()

	return
	# display the graph of header dependencies
	root = os.path.abspath('.')
	def rep(x):
		return str(x).replace(root, '')

	import types
	f = open('me.dot', 'w')
	f.write("digraph G {\n")
	table = bld.depends_on
	for a in table:
		for x in table[a]:
			if type(table[a][x]) is types.ListType:
				for y in table[a][x]:
					f.write('"%s" -> "%s";\n' % (rep(x), rep(y)))
			else:
				f.write('"%s" -> "%s";\n' % (rep(x), rep(table[a][x])))
	f.write('}\n')
	f.close()

	try:
		os.popen('dot -Tpng me.dot -ofoo.png').read()
	except:
		print "the dot program was not found - install graphviz?"
	else:
		print "generated foo.png"


