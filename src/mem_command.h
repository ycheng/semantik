// Thomas Nagy 2007-2011 GPLV3

#ifndef MEM_COMMAND
#define MEM_COMMAND

#include <QPoint>
#include <QSet>
#include <QList>

class data_item;
class sem_model;

class mem_command {
	public:
		mem_command();
};

class mem_delete : public mem_command {
	public:
		mem_delete();
		void init(sem_model* mod, QList<int> ids);

		QList<data_item*> items;
		QSet<QPoint> links;
};

#endif

