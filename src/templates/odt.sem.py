#sem:name: OpenDocument Text
#sem:tip: Generates text documents for Kword or OpenOffice.org Writer

# Thomas Nagy, 2007-2012

mimetype = "application/vnd.oasis.opendocument.text"

import os, shutil, time, zipfile, sys, StringIO

outdir = sembind.get_var('outdir')+'/'+sembind.get_var('pname')

globals = {
'DATE':'<text:date style:data-style-name="N37"></text:date>',
'PAGENUM':'<text:page-number text:select-page="current">1</text:page-number>/<text:page-count>1</text:page-count>',
#'AUTHOR':'<text:initial-creator/>', # <- i wish i could find this one in the gui
#'SUBJECT':'<text:subject/>',
}

settings = {
'doc_content':'',
'manif':'',
'oolang':'fr_FR',
'piclst':[],
'author':'',
'header_l':'@AUTHOR@',
'header_c':'',
'header_r':'@TITLE@',
'footer_l':'@DATE@',
'footer_c':'@PAGENUM@',
'footer_r':'',
}
add_globals(settings)


def p(s):
	return sembind.protectHTML(s)

def x(s):
	return sembind.protectXML(s)

# substitute @var@, look if there is not a global variable named like that in the globals already
# TODO: use a real parser, but for the moment quote every string
for a in settings.keys():
	val = settings[a]
	if len(val)>3:
		if val[0]=='@' and val[-1]=='@' and not ' ' in val:
			nvar = val[1:-1]
			if nvar in settings: settings[a] = x(settings[nvar])
			else: settings[a] = globals.get(nvar, '')

try:
	tm = time.strftime('.%d-%b-%y--%H-%M-%S', time.gmtime(os.stat(outdir).st_mtime))
	os.rename(outdir, outdir+tm)
except OSError:
	pass

try: os.makedirs(outdir)
except OSError: debug("Cannot create folder " + outdir)

try: os.makedirs(outdir+'/Pictures')
except OSError: debug("Cannot create folder " + outdir)

# copy the pictures
cwd = os.getcwd()
os.chdir(sembind.get_var('temp_dir'))
pics = {} # map the id to the picture
lst = os.listdir('.')
for x in lst:
	if x.startswith('diag-'):
		pics[ x.replace('diag-', '').split('.')[0] ] = x
		shutil.copy2(x, outdir + '/Pictures')
os.chdir(cwd)

try: os.mkdir(outdir+'/META-INF')
except: raise


buf = []
out = buf.append

from sgmllib import SGMLParser
import htmlentitydefs

class TrucProcessor(SGMLParser):
	def reset(self):
		self.pieces = []
		self.state = ""
		self.buf = ""
		self.inli = 0
		SGMLParser.reset(self)

	def unknown_starttag(self, tag, attrs):
		if tag == 'ul':
			if self.inli and self.buf:
				self.pieces.append('<text:p text:style-name="P1">')
				self.pieces.append(self.buf)
				self.pieces.append('</text:p>')
			self.pieces.append('<text:list text:style-name="L2">\n')

		if tag == 'li':
			self.pieces.append('<text:list-item>')
			self.inli += 1

		self.buf = ""

	def unknown_endtag(self, tag):
		if tag == 'p':
			self.pieces.append('<text:p text:style-name="Standard">')
			self.pieces.append(self.buf)
			self.pieces.append('</text:p>\n')
		elif tag == 'li':
			if self.buf:
				self.pieces.append('<text:p text:style-name="P1">')
				self.pieces.append(self.buf)
				self.pieces.append('</text:p>\n')
			self.pieces.append('</text:list-item>\n')
			self.inli -= 1
		elif tag == 'ul':
			self.pieces.append('</text:list>\n')

	def handle_charref(self, ref):
		self.pieces.append("&#%(ref)s;" % locals())

	def handle_data(self, text):
		self.buf = text

	def output(self):
		return "".join(self.pieces)

def parse_string(s):
	parser = TrucProcessor()
	parser.feed(s)
	parser.close()
	return parser.output()

def print_nodes(node, niv, lbl_lst):

	lbl = ".".join(lbl_lst)

	typo = node.get_val('type')

	txt = x(node.get_val('summary'))
	#out('<text:h text:style-name="Heading_x_%d" text:outline-level="%d">%s</text:h>\n' % (nv, nv, txt))

	if niv == 0:
		settings['doc_title'] = node.get_val('summary')
	elif niv in range(8) and typo == 'text':

		for xxx in range(niv):
			if xxx == 0:
				out('<text:list text:style-name="L1" text:continue-numbering="true">')
			else:
				out('<text:list text:continue-numbering="true">')
			out('<text:list-item>')

		#out('<text:p text:style-name="P1">%s</text:p>' % txt)
		out('<text:h text:style-name="Heading_x_%d" text:outline-level="%d">%s</text:h>\n' % (niv, niv, txt))

		for xxxx in range(niv):
			out('</text:list-item>')
			out('</text:list>')

		#out('<text:h text:style-name="Heading_x_%d" text:outline-level="%d">%s %s</text:h>\n' % (niv, niv, lbl, txt))

	if typo == 'text':
		y = node.get_val('text')
		if y: out(parse_string(y))

	elif typo == 'table':
		rows = node.num_rows()
		cols = node.num_cols()
		if rows>0 and cols>0 and not node.get_var('exclude_table'):

			caption = node.get_var('tbl_caption')
			if not caption: caption = '(TODO: set a caption for this table! -> var tbl_caption)'

			out('\n')
			out('<table:table table:name="Tableau1" table:style-name="Tableau1">\n')
			#out('<caption>%s</caption>\n' % x(caption))
			out('<table:table-column table:style-name="Tableau1.A" table:number-columns-repeated="%d"/>\n' % cols)
			for i in range(rows):
				out('\t<table:table-row>\n')
				for j in range(cols):
					if i>0 and j>0:
						out('\t\t<table:table-cell table:style-name="Tableau1.B2" office:value-type="string">\n')
						out('\t\t\t<text:p text:style-name="P1">%s</text:p>\n' % x(node.get_cell(i, j)))
						out('\t\t</table:table-cell>\n')
					else:
						out('\t\t<table:table-cell table:style-name="Tableau1.B1" office:value-type="float" office:value="0">\n')
						out('\t\t\t<text:p text:style-name="P1">%s</text:p>\n' % x(node.get_cell(i, j)))
						out('\t\t</table:table-cell>\n')
				out('\t</table:table-row>\n')
			out('</table:table>\n')

		out('\n')

	elif typo == 'pic' or typo == 'diag':
		id = node.get_val('id')
		if id in pics and not node.get_var('exclude_pic'):

			caption = node.get_var('pic_caption')
			if not caption: caption = '(TODO: set a caption for this picture!)'

			p = pics[id]

			#out('<draw:frame draw:style-name="fr1" text:anchor-type="paragraph" draw:z-index="0">\n')
			#out('<draw:text-box min-height="3cm">\n')
			#out('<text:p text:style-name="Illustration">\n')

			#out('<draw:frame text:anchor-type="paragraph" style:rel-height="scale" draw:z-index="1" svg:width="5cm">')
			#out('<draw:image xlink:href="Pictures/%s" xlink:type="simple" xlink:show="embed" xlink:actuate="onLoad"/>' % pics[id])
			#out('</draw:frame>\n')

			#out('</text:p>\n')
			#out('</draw:text-box>\n')
			#out('</draw:frame>\n')

			#out('<text:p text:style-name="Standard">\n')
			#out('<draw:frame text:anchor-type="paragraph" style:rel-height="scale" draw:z-index="1">\n')
			#out('<draw:image xlink:href="Pictures/pic-6.jpg" xlink:type="simple" xlink:show="embed" xlink:actuate="onLoad"/>\n')
			#out('</draw:frame>\n')
			#out('</text:p>\n')

			out('<text:p text:style-name="Standard">\n')
			out('<draw:frame draw:style-name="fr1" draw:name="Image1" text:anchor-type="paragraph" ')
			#out(' svg:width="5cm" svg:height="5cm" draw:z-index="0">\n')
			out(' draw:z-index="0">\n')
			out('<draw:image xlink:href="Pictures/%s" xlink:type="simple" xlink:show="embed" xlink:actuate="onLoad"/>\n' % p)
			out('</draw:frame>\n')
			out('</text:p>\n')


			t = 'image/jpeg'

			if p.find('jpg')>0 or p.find('jpeg')>0: t = 'image/jpeg'
			elif p.find('png')>0: t = 'image/png'
			elif p.find('gif')>0: t = 'image/gif'

			settings['manif'] += '<manifest:file-entry manifest:media-type="%s" manifest:full-path="Pictures/%s"/>' % (t, p)
			settings['piclst'].append(p)

	num = node.child_count()
	for i in range(num):
		print_nodes(node.child_num(i), niv+1, lbl_lst+[str(i+1)])

# the main content
print_nodes(Root(), 0, []);
settings['doc_content'] = "".join(buf)

# prepare files
transform("/odt/manifest.xml", outdir+'/META-INF/manifest.xml', settings)
for x in "content.xml settings.xml styles.xml meta.xml".split():
	transform("/odt/"+x, outdir+'/'+x, settings)

# add files to the zip
file = zipfile.ZipFile(outdir+'/main.odt', mode='w')

os.chdir(outdir)
file.writestr('mimetype', mimetype)
for x in "content.xml styles.xml meta.xml".split():
	file.write(x)#, compress_type=zipfile.ZIP_DEFLATED)
file.write('META-INF/manifest.xml')#, compress_type=zipfile.ZIP_DEFLATED)
for x in settings['piclst']:
	file.write('Pictures/%s' % x)

file.close()

# and remove the useless stuff
os.popen('cd %s && rm -rf *.xml META-INF Pictures' % outdir)

visualize('odt', outdir+'/main.odt')

