#! /usr/bin/env python
# encoding: utf-8
# Thomas Nagy, 2007 (ita)

import os, sys, tarfile, StringIO
from xml.sax import make_parser
from xml.sax.handler import ContentHandler

def debug(s):
	sys.stderr.write(s)

def protect(t):
	lst = t.split('&')
	t = "&amp;".join(lst)
	lst = t.split('<')
	t = "&lt;".join(lst)
	lst = t.split('>')
	t = "&gt;".join(lst)
	lst = t.split('"')
	t = "&quot;".join(lst)
	return t

head = """<?xml version="1.0" encoding="utf8"?>
<semantik version="1">
<color_schemes>
  <color_scheme name="Color 0" inner="#fffe8d" border="#000000" text="#000000"/>
  <color_scheme name="Color 1" inner="#91ffab" border="#000000" text="#000000"/>
  <color_scheme name="Color 2" inner="#9bfffe" border="#000000" text="#000000"/>
  <color_scheme name="Color 3" inner="#b8bbff" border="#000000" text="#000000"/>
  <color_scheme name="Color 4" inner="#e0aaff" border="#000000" text="#000000"/>
  <color_scheme name="Color 5" inner="#ffa6a6" border="#000000" text="#000000"/>
  <color_scheme name="Color 6" inner="#ffd8a6" border="#000000" text="#000000"/>
  <color_scheme name="Color 7" inner="#ffffff" border="#000000" text="#000000"/>
</color_schemes>
"""

textitem = """<item id="%s" summary="%s" text="%s" len="13" comment="%s" pic_location="" pic_caption="" pic_comment="" tbl_rows="0" tbl_cols="0" c1="%s" c2="%s" color="1" custom_name="" custom_border="#000000" custom_inner="#000000" custom_text="#000000">
<tblsettings rows="0" cols="0"/>
</item>
"""

lst_vars = ['id', 'summary', 'text', 'len', 'comment', 'pic_location', 'pic_caption', 'pic_comment', 'tbl_rows', 'tbl_cols', 'c1', 'c2', 'color', 'custom_name', 'custom_border', 'custom_inner', 'custom_text']

rep = {
	'text':'text',
	'summary':'summary',
	'xpos':'c2',
	'ypos':'c1',
	'id':'id',
}

class QTXMLHandler(ContentHandler):
	def __init__(self):
		self.buf = []
		self.out = []
		self.trucs = []

		self.out.append(head)

	def init_tbl(self):
		self.tbl = {}
		g=self.tbl
		for i in lst_vars:
			g[i] = ''

	def startElement(self, name, attrs):
		self.links = []
		self.buf = []
		if name == 'meta':
			def g(x):
				return attrs.get(x, '')

			#fname = g('fname')
			#sname = g('sname')
			#email = g('email')
			#organ = g('organization')
			self.out.append('<info hints=""/>')
		elif name == 'item':
			self.tbl = {}
		else:
			self.buf = []

	def endElement(self, name):
		txt = "".join(self.buf)
		if name == 'item':
			self.out.append('<item ')

			for x in lst_vars:
				value = self.tbl.get(x, '')
				if x == 'id': value = str(int(value)+1) # offset +1
				self.out.append(' %s="%s"' % (x, protect(value)))

			self.out.append('/>\n')
		elif name == 'child':
			self.trucs.append([1+int(self.tbl['id']), 1+int("".join(self.buf))])
		elif name in rep:
			#debug("found name %s \n" % name)
			self.tbl[rep[name]]=txt
		elif name == 'kdissertdoc':

			for i in self.trucs:
				self.out.append('<link p="%d" v="%d"/>' % (i[0], i[1]))

			self.out.append('</semantik>')

	def characters(self, cars):
		self.buf.append(cars)

def parse_string(s):
	parser = make_parser()
	curHandler = QTXMLHandler()
	parser.setContentHandler(curHandler)
	parser.parse(StringIO.StringIO(str(s)))
	return "".join(curHandler.out)

def parse_file(infile):
	tar = tarfile.open(infile)
	for tarinfo in tar:
		tar.extract(tarinfo)

	# offset +1
	lst = os.listdir('.')
	map = {}
	for x in lst:
		if x[:4] != 'pic-': continue
		y = (x[4:].split('.'))[0]
		map[y] = x
	k = map.keys()
	k.sort()
	k.reverse()
	for u in k:
		name = map[u]
		ext = name.split('.')[1]
		os.rename(name, 'pic-%d.%s' % (int(u)+1, ext))

	tar.close()

	file = open('maindoc.xml', 'rb')
	txt = file.read()
	file.close()
	os.remove('maindoc.xml')

	truc = txt.replace('<?xml version="1.0" encoding="utf8"?>', '<?xml version="1.0" encoding="UTF-8"?>')
	truc = parse_string(truc)

	#file = open("/tmp/con.xml", "w")
	#file.write(truc)
	#file.close()
	#debug(str(type(truc)))

	return truc.encode('utf-8')

