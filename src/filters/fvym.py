#! /usr/bin/env python
# encoding: utf-8
# Thomas Nagy, 2007-2014 (ita)

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

class FFHandler(ContentHandler):
	def __init__(self):
		self.buf = []
		self.out = []
		self.trucs = []

		self.cur = 0

		self.count = 1
		self.ids = [] # stack
		self.links = [] # tuples
		self.headings={}

		self.ids.append(self.count)

	def startElement(self, name, attrs):
		self.buf = []
		if name == 'vymmap':
			self.out.append(head)

		if name == 'branch':
			self.count += 1
			#self.cur += 1
			#debug(str(self.cur))
			id = self.count

			if len(self.ids) > 0:
				par = self.ids[-1]
				self.links.append( (par, id) )

			self.ids.append(id)

	def endElement(self, name):
		txt = "".join(self.buf)
		if name == 'branch' or name == 'mapcenter':
			#self.cur -= 1
			#debug(str(self.cur))
			id = self.ids[-1]

			summary = self.headings[id]
			self.out.append('<item id="%d" summary="%s"/>\n' % (id, summary))

			self.ids=self.ids[:-1]

		elif name == 'vymmap':
			for (k, v) in self.links:
				self.out.append('<link p="%d" v="%d"/>\n' % (k, v))

			self.out.append('</semantik>')
		elif name == 'heading':
			# be careful with the order
			id = self.ids[-1]
			self.headings[id] = txt
		elif name == 'htmlnote':
			# fuck, we have to parse the html code here :-/
			pass

	def characters(self, cars):
		self.buf.append(cars)

def parse_string(s):
	parser = make_parser()
	curHandler = FFHandler()
	parser.setContentHandler(curHandler)
	parser.parse(StringIO.StringIO(str(s)))
	return "".join(curHandler.out)

def parse_file(infile):
	import zipfile
	zut = zipfile.ZipFile(infile, "r")
	truc = None
	for filename in zut.namelist():
		if filename.endswith('.xml'):
			truc = zut.read(filename)

	truc = truc.replace('<?xml version="1.0" encoding="utf8"?>', '<?xml version="1.0" encoding="UTF-8"?>')
	truc = parse_string(truc)

	#file = open("/tmp/con.xml", "w")
	#file.write(str(truc))
	#file.close()
	#debug(truc.encode('utf-8'))

	return truc.encode('utf-8')

