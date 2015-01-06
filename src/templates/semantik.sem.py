#sem:name: semantik document generator
#sem:tip: used for saving semantik documents

# Thomas Nagy, 2007-2015

import os, tarfile
from cStringIO import StringIO

tar = tarfile.open(sembind.get_var('outfile'), 'w:gz')

doc = sembind.get_var('fulldoc')
stuff = StringIO(doc)
tarinfo = tarfile.TarInfo('con.xml')
tarinfo.size = len(doc)
tar.addfile(tarinfo, stuff)

#debug(doc.encode('utf-8'))

os.chdir(sembind.get_var('temp_dir'))
lst = os.listdir('.')

tmp = str(sembind.get_item_ids()).split(",")
for x in tmp:
	item = sembind.get_item_by_id(int(x))
	if item:
		pid = sembind.get_val(item, "pic_id")
		s = 'img-%s' % pid
		if pid != '0':
			for pic in lst:
				if pic.startswith(s):
					tar.add(pic, pic)
					break
			else:
				debug("Could not find the picture for item %r->%r in %r" % (x, pid, sembind.get_var('temp_dir')))
	else:
		debug("Could not get an object for %r" % x)
tar.close()

