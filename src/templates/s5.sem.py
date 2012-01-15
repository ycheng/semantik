#sem:name: S5 presentation
#sem:tip: Generates a xhtml presentation using s5

# Thomas Nagy, 2007-2012

import os, shutil, time

outdir = sembind.get_var('outdir')+'/'+sembind.get_var('pname')

settings = {
'doc_date':'',
'doc_content':'',
'doc_title':'How i did it',
'doc_author':'Krusty',
'doc_place':'Los angeles',
'doc_company':'World company'
}
add_globals(settings)

try:
	tm = time.strftime('.%d-%b-%y--%H-%M-%S', time.gmtime(os.stat(outdir).st_mtime))
	os.rename(outdir, outdir+tm)
except OSError:
	pass

try: os.makedirs(outdir+'/ui/default/')
except OSError: debug('Cannot create folder ' + outdir + '/ui/default/')

# copy the pictures
cwd = os.getcwd()
os.chdir(sembind.get_var('temp_dir'))
pics = {} # map the id to the picture
lst = os.listdir('.')
for x in lst:
	if x.startswith('diag-'):
		pics[ x.replace('diag-', '').split('.')[0] ] = x
		shutil.copy2(x, outdir)
os.chdir(cwd)


buf = []
out = buf.append

def p(s):
	return sembind.protectHTML(s)

def x(s):
	return sembind.protectXML(s)

def print_bullet(node, indent):
	out('<li>%s</li>\n' % node.get_val('summary'))
	y = node.get_val('text')
	out('<div class="handout">%s</div>\n' % p(y))

	num = node.child_count()
	if num:
		out('<ul>\n')
		for i in range(num):
			print_bullet(node.child_num(i), i)
		out('</ul>\n')

def print_slide(node, niv):
	out('\n\n<div class="slide">\n')

	out('<h1>%s</h1>\n' % node.get_val('summary'))
	out('<div class="handout">%s</div>\n' % p(node.get_val('text')))

	num = node.child_count()
	if num:
		out('<ul>\n')
		for i in range(num):
			print_bullet(node.child_num(i), i)
		out('</ul>\n')


	out('</div>\n')

def print_nodes(node, niv):

	has_child_skip = 0
	num = node.child_count()
	for i in range(num):
		if node.child_num(i).get_var('make_slide'):
			has_child_skip = 1
			break

	if node.get_var('make_slide'):
		print_slide(node, niv)

	# stupid heuristic
	elif has_child_skip or (int(node.get_val('tree_size')) > 12):
		if not node.get_var('skip_slide'):
			out('\n\n<div class="slide">\n')
			out('<h1>%s</h1>\n' % node.get_val('summary'))
			out('<div class="handout">%s</div>\n' % p(node.get_val('text')))

			# print the main titles
			num = node.child_count()
			if num:
				out('<ul>\n')
				for i in range(num):
					tx = node.child_num(i).get_val('summary')
					out('<li>%s</li>\n' % tx)
					out('<div class="handout"></div>\n')
				out('</ul>\n')

			out('</div>\n')

		num = node.child_count()
		for i in range(num):
			print_nodes(node.child_num(i), niv+1)
	else:
		print_slide(node, niv)

	'''
	rows = node.num_rows()
	cols = node.num_cols()
	if rows>0 and cols>0:
		out('\n')
		out('<table class="sem_table" cellspacing="0px" cellpadding="0px">\n')
		out('<tbody>\n')
		for i in range(rows):
			out('\t<tr>\n')
			for j in range(cols):
				if i>0 and j>0:
					out('\t\t<td>%s</td>\n' % x(node.get_cell(i, j)))
				else:
					out('\t\t<th>%s</th>\n' % x(node.get_cell(i, j)))
			out('\t</tr>\n')

		out('</tbody>\n')
		out('</table>\n')
	out('\n')
	'''

# the main document
print_nodes(Root(), 0);
settings['doc_content'] = "".join(buf)

# now write the document
transform("/s5/index.html", outdir+'/index.html', settings)

files_lst = """
framing.css
iepngfix.htc
opera.css
outline.css
pretty.css
print.css
s5-core.css
slides.css
slides.js
""".split()

shutil.copy2(template_dir()+'/s5/ui/default/blank.gif', outdir+'/ui/default/')
for x in files_lst:
	transform('/s5/ui/default/'+x, outdir+'/ui/default/'+x, settings)

visualize('s5', outdir+'/index.html')


