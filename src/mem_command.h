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
		mem_command(sem_model*);
		sem_model *model;
		virtual void undo() = 0;
		virtual void redo() = 0;
		void apply();
};

class mem_delete : public mem_command {
	public:
		mem_delete(sem_model*);
		void init(QList<int> ids);
		void undo();
		void redo();

		QList<data_item*> items;
		QSet<QPoint> links;
};

class mem_add : public mem_command {
	public:
		mem_add(sem_model*);
		void init();
		void undo();
		void redo();

		data_item* item;
};

#endif

