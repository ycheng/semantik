#sem:name: semantik document generator
#sem:tip: used for saving semantik documents

# Thomas Nagy, 2007-2012

import os, tarfile
from cStringIO import StringIO

tar = tarfile.open(sembind.get_var('outfile'), 'w:gz')

doc = sembind.get_var('fulldoc')
stuff = StringIO(doc)
#stuff = StringIO(doc)
tarinfo = tarfile.TarInfo('con.xml')
tarinfo.size = len(doc)
tar.addfile(tarinfo, stuff)

#debug(doc.encode('utf-8'))

os.chdir(sembind.get_var('temp_dir'))

tmp = str(sembind.get_var('pics')).split(",")
lst = os.listdir('.')
for name in lst:
	f = name.split('.')[0].replace('pic-', '')
	if f in tmp:
		tar.add(name, name)
tar.close()

