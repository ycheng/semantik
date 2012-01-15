#sem:name: PDF LaTeX
#sem:tip: Generates a LaTeX project for obtaining PDF files

# Thomas Nagy, 2007-2012

import os, time, shutil, re, StringIO
from sgmllib import SGMLParser
import htmlentitydefs

# Additional variables:
# exclude   1
# picdim    [width=10cm]

settings = {
'doc_content':'',
'doc_class':'article',

'all_latex':False,
'header_off':'',
'footer_off':'',
'header_l':'',
'header_c':'',
'header_r':'',
'footer_l':'',
'footer_c':r'\thepage/\pageref{LastPage}',
'footer_r':r'\today',

'doc_title':'',
'doc_title_off':'None',

'doc_date':'',
'doc_date_off':'None',

'doc_author':'',
'doc_author_off':'None',

'babel':'english', # frenchb
}
add_globals(settings)


outdir = sembind.get_var('outdir')+'/'+sembind.get_var('pname')

try:
	tm = time.strftime('.%d-%b-%y--%H-%M-%S', time.gmtime(os.stat(outdir).st_mtime))
	os.rename(outdir, outdir+tm)
except OSError:
	pass

try:
	os.makedirs(outdir)
except OSError:
	debug("Cannot create folder " + outdir)


# do the document in latex ?
def tex_convert(s):
	return s
if not settings.get('all_latex', 0): tex_convert = protect_tex

# make the pictures available
cwd = os.getcwd()
os.chdir(sembind.get_var('temp_dir'))
pics = {} # map the id to the picture
lst = os.listdir('.')
for x in lst:
	if x.startswith('diag-'):
		pics[ i.replace('diag-', '').split('.')[0] ] = x
		shutil.copy2(x, outdir)
os.chdir(cwd)

buf = []
out = buf.append

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
				self.pieces.append('\\item ')
				self.pieces.append(tex_convert(self.buf))
				self.pieces.append('\n')
			self.pieces.append('\\begin{itemize}\n')

		if tag == 'li':
			self.inli += 1

		self.buf = ""

	def unknown_endtag(self, tag):
		if tag == 'p':
			self.pieces.append(tex_convert(self.buf))
			self.pieces.append('\n')
		elif tag == 'li':
			if self.buf:
				self.pieces.append('\\item ')
				self.pieces.append(tex_convert(self.buf))
				self.pieces.append('\n')
			self.inli -= 1
		elif tag == 'ul':
			self.pieces.append('\\end{itemize}\n')

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

def print_nodes(node, niv):
	sm = tex_convert(node.get_val('summary'))
	if settings['doc_class'] in ['book', 'report']:
		if niv == 1:
			out('\\chapter{%s}\n' % sm)
		elif niv == 2:
			out('\\section{%s}\n' % sm)
		elif niv == 3:
			out('\\subsection{%s}\n' % sm)
		elif niv == 4:
			out('\\subsubsection{%s}\n' % sm)
		elif niv == 5:
			out('\\paragraph{%s}\n' % sm)
	else:
		if niv == 1:
			out('\\section{%s}\n' % sm)
		elif niv == 2:
			out('\\subsection{%s}\n' % sm)
		elif niv == 3:
			out('\\subsubsection{%s}\n' % sm)
		elif niv == 4:
			out('\\paragraph{%s}\n' % sm)

	typo = node.get_val('type')
	if typo == 'text':
		y = node.get_val('text')
		out(parse_string(y))

	elif typo == 'table':
		rows = node.num_rows()
		cols = node.num_cols()
		if rows>0 and cols>0:

			caption = node.get_var('caption')
			if not caption: caption = caption = node.get_val('summary')

			out('\\begin{table}\n')

			out('\\begin{center}\n')
			out('\\begin{tabular}{|%s}' % ('c|'*cols))
			out(' \\hline\n')
			for i in range(rows):
				for j in range(cols):
					if i == 0 or j == 0:
						out('\\textbf{%s}' % tex_convert(node.get_cell(i, j)))
					else:
						out('%s' % tex_convert(node.get_cell(i, j)))
					if j < cols - 1: out(" & ")
				out(' \\\\ \\hline\n')
			out('\\end{tabular}\n')
			out('\\end{center}\n')

			out('\\caption{%s}\n' % tex_convert(caption))
			out('\\end{table}\n')

		out('\n')

	elif typo == 'img' or typo == 'diag':
		id = node.get_val('id')
		if id in pics:

			caption = node.get_var('caption')
			if not caption: caption = caption = node.get_val('summary')

			restrict = node.get_var("picdim")
			if not restrict:
				w = int(node.get_val('pic_w'))
				restrict = ""
				if (w > 5*72): restrict = "[width=5in]"

			out('\\begin{figure}[htbp]\n')
			out('  \\begin{center}\n')
			out('    \\includegraphics%s{%s}\n' % (restrict, pics[id]))
			out('    \\caption{\\footnotesize{%s}}\n' % tex_convert(caption))
			out('%% %s\n' % protect_tex(node.get_val('pic_location')))
			out('%% %s\n' % node.get_val('pic_w'))
			out('%% %s\n' % node.get_val('pic_h'))
			out('    \\end{center}\n')
			out('\\end{figure}\n')

	num = node.child_count()
	for i in range(num):
		print_nodes(node.child_num(i), niv+1)

root = Root()

title = root.get_val('title')
if title:
	settings['doc_title_off']=''
	settings['doc_title']=title

date = r'\today' #root.get_val('date')
if date:
	settings['doc_date_off']='None'
	settings['doc_date']=date

author = root.get_val('pname')
if author:
	settings['doc_author_off']=''
	settings['doc_author']=author

# the main document
print_nodes(root, 0);
settings['doc_content'] = "".join(buf)

# now write main.tex
transform("/pdflatex/main.tex", outdir+'/main.tex', settings)

# anciliary files
shutil.copy2(template_dir()+'/pdflatex/wscript', outdir+'/wscript')
shutil.copy2(template_dir()+'/waf', outdir+'/waf')
os.chmod(outdir+'/waf', 0755)

# load the preview on main.tex
visualize('pdflatex', outdir+'/main.tex')


