#! /usr/bin/env python
# encoding: utf-8
# Thomas Nagy, 2007-2012

import sys, sembind, re

protectXML = sembind.protectXML
sys.path = [sembind.get_var('template_dir')]+sys.path

if not hasattr(sys, "argv"): sys.argv = ['semantik']
#sys.stderr.write(str(sys.argv))

def subst_vars(doc, map):
	ugh = '-<[{($)}]>-'
	tmp = doc.replace('%', ugh)
	m4_re = re.compile('@(\w+)@', re.M)
	tmp = m4_re.sub(r'%(\1)s', tmp)
	tmp = tmp % map
	tmp = tmp.replace(ugh, '%')
	return tmp

def visualize(type, doc):
	var = 'command_'+type
	if var in settings:
		os.popen(settings[var] % doc).read()
	else:
		sembind.set_var('preview', doc)

def read_properties(code):
	tmp={}
	for x in code.split('\n'):
		if not x: continue
		if x[0]=="#": continue
		lst = x.split("=")
		if len(lst) < 2: continue
		tmp[lst[0]] = "=".join(lst[1:])
	return tmp

def template_dir():
	return sembind.get_var('template_dir')
#TEMPLATE_DIR = sembind.get_var('template_dir')
#FILTER_DIR = sembind.get_var('filter_dir')
#GLOBAL_VARS = read_properties(sembind.get_var('hints'))

def post_process(txt, defines):
	nested = 0
	nested_lst = range(30)
	nested_lst[0]=1

	lst = txt.split('\n')
	out = []

	def evaluate(txt):
		negate = 0
		ret = 0
		if len(txt)>1 and txt[0] == '!':
			txt = txt[1:]
			negate = -1
		try:
			num = int(txt)
			if num: ret=1
		except:
			if txt in defines:
				if defines[txt]:
					ret = 1
		ret = ret+negate
		#if ret<0: ret = 1
		return ret

	for x in lst:
		if x.find('#endif')==0:
			nested -= 1
		elif x.find('#if ')==0:
			nested += 1
			txt = x.replace('#if ', '')
			nested_lst[nested] = evaluate(txt)
		#elif x.find('#elif ')==0:
		#	if nested_lst[nested]:
		#		nested_lst[nested] = "skip"
		#		continue
		#	txt = x.replace('#elif ', '')
		#	nested_lst[nested] = evaluate(txt)
		elif x.find('#ifdef ')==0:
			nested += 1
			txt = x.replace('#ifdef ', '').rstrip()
			nested_lst[nested] = (txt in defines)
		elif x.find('#ifndef ')==0:
			nested += 1
			txt = x.replace('#ifndef ', '').rstrip()
			nested_lst[nested] = not (txt in defines)
		elif x.find('#else')==0:
			if nested_lst[nested]:
				nested_lst[nested]=0
			else:
				nested_lst[nested]=1
		else:
			if nested_lst[nested]:
				out.append(x)
	return "\n".join(out)

def write_to_file(name, content):
	if sys.platform.rfind('32') > -1: return
	file = open(name, 'wb')
	file.write(content)
	file.close()

def read_file(name):
	file = open(name, 'r')
	doc = file.read()
	file.close()
	return doc

def add_globals(table):
	GLOBAL_VARS = read_properties(sembind.get_var('hints'))
	template_name = sembind.get_var('namet').split('/')[-1].replace('.sem.py', '')
	for x in GLOBAL_VARS:
		table[x] = GLOBAL_VARS[x]
		lst = x.split('.')
		# override for sub-commands
		if len(lst)>1 and lst[1] == template_name:
			table[lst[0]] = GLOBAL_VARS[x]

def transform(template, outfile, map):
	doc = read_file(template_dir() + template)
	doc = subst_vars(doc, settings)
	doc = post_process(doc, settings)
	write_to_file(outfile, doc)

def debug(x):
	try:
		#sys.stderr.write(x.__repr__())
		sys.stderr.write(x)
		sys.stderr.write("\n")
	except:
		debug("erreur d'encodage utf8 ? :-/")

def protect_tex(s):
	lst = []
	for x in s:
		if x == '\\': lst.append(r'$\backslash$')
		elif x == '#': lst.append(r'\#')
		elif x == '$': lst.append(r'\$')
		elif x == '%': lst.append(r'\%')
		elif x == '&': lst.append(r'\&')
		elif x == '_': lst.append(r'\_')
		elif x == '{': lst.append(r'\{')
		elif x == '}': lst.append(r'\}')
		elif x == '^': lst.append(r'\^{}')
		elif x == '~': lst.append(r'\~{}')
		elif x == '<': lst.append('$<$')
		elif x == '>': lst.append('$>$')
		else: lst.append(x)
	return "".join(lst)

class Node:
	def __init__(self, bind):
		if not bind:
			debug("error!! node takes a parameter")
			sys.exit(1)
		self.ptr = bind
		self.vars = {}

	def get_val(self, s):
		#debug("get_val called %s" % s)
		if s == "type":
			ret = sembind.get_val(self.ptr, s)
			if ret == '3': return 'text'
			if ret == '4': return 'img'
			if ret == '5': return 'diag'
			if ret == '6': return 'table'
			return ret
		return sembind.get_val(self.ptr, s)

	def get_var(self, s):
		if not self.vars:
			hints = sembind.get_val(self.ptr, "hints")
			self.vars = read_properties(hints)
		return self.vars.get(s, "")

	def child_count(self):
		#debug(str(self.ptr))
		return sembind.child_count(self.ptr)

	def child_num(self, num):
		v = sembind.child_num(self.ptr, num)
		return Node(v)

	def num_rows(self):
		return sembind.num_rows(self.ptr)

	def num_cols(self):
		return sembind.num_cols(self.ptr)

	def get_cell(self, row, col):
		return sembind.get_cell(self.ptr, row, col)

	def link_count(self):
		# FIXME
		return 0

	def link_caption(self, idx):
		# FIXME
		return "SEMANTIK: link_caption is missing"

	def link_location(self, idx):
		# FIXME
		return "SEMANTIK: link_location is missing"

def Root():
	return Node(sembind.instance())

