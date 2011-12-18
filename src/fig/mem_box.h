// Thomas Nagy 2007-2011 GPLV3

#ifndef MEM_BOX
#define MEM_BOX

#include "mem_command.h"

class mem_del_box : public mem_command {
	public:
		mem_del_box(sem_model*);
		void init(int id, QList<int> ids);
		void undo();
		void redo();

		//QList<data_item*> items;
		//QSet<QPoint> links;

		data_item *item;

		QList<data_item*> items;
		QSet<QPoint> links;
	
		IType type() { return DEL_BOX; }
};


class mem_add_box : public mem_command {
	public:
		mem_add_box(sem_model*);
		void init(int id);
		void undo();
		void redo();

		data_item* item;
		data_box* box;

		double m_iXX;
		double m_iYY;

		IType type() { return ADD_BOX; }
};

#endif
