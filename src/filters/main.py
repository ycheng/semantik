#! /usr/bin/env python
# encoding: utf-8
# Thomas Nagy, 2007 (ita)

import os, sys, tarfile

filters = sembind.get_var('filter_dir')
sys.path = [filters]+sys.path

tmpdir = sembind.get_var('temp_dir')
infile = sembind.get_var('pname')

def debug(s):
	sys.stderr.write(s)

os.chdir(tmpdir)

ret = ''
name = infile.lower()
if sys.platform == 'win32' or sys.platform == 'darwin':
	ret = ''
elif name.endswith('.kdi'):
	import kdissert
	ret = kdissert.parse_file(infile)
elif name.endswith('.sem'):
	import semantik
	ret = semantik.parse_file(infile)
elif name.endswith('.mm'):
	import others
	ret = others.parse_file(infile)
elif name.endswith('.vym'):
	import fvym
	ret = fvym.parse_file(infile)

sembind.set_var("fulldoc", ret)

