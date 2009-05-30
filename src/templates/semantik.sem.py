#sem:name: semantik document generator
#sem:tip: used for saving semantik documents

# Thomas Nagy, 2007 (ita)
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
lst = os.listdir('.')
for i in lst:
	if -1 == i.rfind('pic-'): continue
	tar.add(i, i)

tar.close()

