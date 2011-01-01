// Thomas Nagy 2007-2011 GPLV3

#include "mem_command.h"
#include "sem_model.h"

mem_command::mem_command() {

}

mem_delete::mem_delete() {

}

inline uint qHash(const QPoint &p) {
	return p.x() + p.y();
}

void mem_delete::init(sem_model* mod, QList<int> lst) {

	foreach (int id, lst) {

		items.append(*mod + id);

		for (int i = 0; i < mod->m_oLinks.size(); i++)
		{
			QPoint p = mod->m_oLinks.at(i);
			if (p.x() == id || p.y() == id)
			{
				links.insert(p);
			}
		}
	}
}




