#! /usr/bin/env python
# encoding: utf-8
# Thomas Nagy, 2007-2014 (ita)

import os, sys, tarfile

filters = sembind.get_var('filter_dir')
sys.path = [filters]+sys.path

tmpdir = sembind.get_var('temp_dir')
infile = sembind.get_var('pname')

def debug(s):
	sys.stderr.write(s)
	sys.stderr.write('\n')
	sys.stderr.flush()

os.chdir(tmpdir)

ret = ''
name = infile.lower()
if sys.platform == 'win32' or sys.platform == 'darwin':
	ret = ''
elif name.endswith('.kdi'):
	import kdissert
	ret = kdissert.parse_file(infile)
elif name.endswith('.sem') or name.endswith('.semd'):
	import semantik
	ret = semantik.parse_file(infile)
	# FIXME null bytes should not be allowed in the first place :-(
	ret = ret.replace('\x00', '')
elif name.endswith('.mm'):
	import others
	ret = others.parse_file(infile)
elif name.endswith('.vym'):
	import fvym
	ret = fvym.parse_file(infile)
else:
	debug('Unsupported file format')

sembind.set_var("fulldoc", ret)

