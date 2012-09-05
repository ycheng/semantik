#sem:name: S5 presentation
#sem:tip: Generates a xhtml presentation using s5

# Thomas Nagy, 2007-2012

import os, shutil, time, getpass

outdir = sembind.get_var('outdir')+'/'+sembind.get_var('pname')

settings = {
'doc_date':'',
'doc_content':'',
'doc_title':'Report',
'doc_author':getpass.getuser(),
'doc_place':'Stockholm',
'doc_company':'Organization'
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
	if x.startswith('diag-') and not x.endswith('pdf'):
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

def print_figure_slides(node, niv):
	typo = node.get_val('type')
	if typo in ['table', 'diag', 'img']:

		out('\n\n<div class="slide">\n')
		out('<h1>%s</h1>\n' % node.get_val('summary'))

		if typo == 'table':
			rows = node.num_rows()
			cols = node.num_cols()
			if rows>0 and cols>0:
				out('\n')
				out('<div style="text-align: center; width: 100%;">\n')
				out('<table class="sem_table" cellspacing="0px" cellpadding="0px" style="width: 100%;">\n')
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
				out('</div>\n')
			out('\n')

		elif typo == 'img' or typo == 'diag':
			id = node.get_val('id')
			if id in pics:

				caption = node.get_var('caption')
				if not caption: caption = caption = node.get_val('summary')
				out('<div style="text-align: center; width: 100%;">\n')
				out("<img src='%s'>\n" % pics[id])
				out('</div>\n')

				"""
				restrict = node.get_var("picdim")
				if not restrict:
					w = int(node.get_val('pic_w'))
					restrict = ""
					if (w > 5*72): restrict = "[width=5in]"
				if not restrict:
					restrict = "[width=\\textwidth,height=\\textheight,keepaspectratio]"

				out('\\begin{figure}[htbp]\n')
				out('  \\begin{center}\n')
				out('    \\includegraphics%s{%s}\n' % (restrict, pics[id]))
				out('    \\caption{\\footnotesize{%s}}\n' % tex_convert(caption))
				out('%% %s\n' % protect_tex(node.get_val('pic_location')))
				out('%% %s\n' % node.get_val('pic_w'))
				out('%% %s\n' % node.get_val('pic_h'))
				out('    \\end{center}\n')
				out('\\end{figure}\n')
				"""

		out("</div>\n")

	num = node.child_count()
	for i in range(num):
		print_figure_slides(node.child_num(i), niv+1)


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
		print_figure_slides(node, niv)

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
semantik.css
""".split()

shutil.copy2(template_dir()+'/s5/ui/default/blank.gif', outdir+'/ui/default/')
for x in files_lst:
	transform('/s5/ui/default/'+x, outdir+'/ui/default/'+x, settings)

visualize('s5', outdir+'/index.html')


