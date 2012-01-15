#sem:name: semantik document generator
#sem:tip: used for saving semantik documents

# Thomas Nagy, 2007-2012

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
	if not item:
		debug("Could not get an object for %r" % tmp)
	else:
		s = 'pic-%s' % item.get_var("pic_id")
		for pic in lst:
			if pic.startswith(s):
				tar.add(pic, pic)
				break
tar.close()

