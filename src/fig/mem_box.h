// Thomas Nagy 2007-2012 GPLV3

#ifndef MEM_BOX
#define MEM_BOX

#include "QHash"
#include "QGraphicsItem"
#include "mem_command.h"

class data_link;

class mem_del_box : public mem_command {
	public:
		mem_del_box(sem_model*, int id);
		void init(QList<data_box*>, QList<data_link*>);
		void undo();
		void redo();

		int m_iId;
		QList<data_box*> items;
		QList<data_link*> links;

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

class mem_edit_box: public mem_command {
	public:
		mem_edit_box(sem_model*, int id, int box);
		void undo();
		void redo();

		data_item *item;
		data_box *box;

		QString oldText;
		QString newText;
		IType type() { return EDIT_BOX; }
};

class mem_link_box : public mem_command {
	public:
		mem_link_box(sem_model*, int id);
		void init(int, int, int, int);
		void undo();
		void redo();

		int m_iId;
		data_link *link;

		IType type() { return LINK_BOX; }
};

class mem_unlink_box : public mem_command {
	public:
		mem_unlink_box(sem_model*, int id);
		void undo();
		void redo();

		int m_iId;
		data_link *link;

		IType type() { return UNLINK_BOX; }
};

#define CH_COLOR 1
#define CH_BORDER 2
#define CH_PENST 4

class mem_prop_box : public mem_command {
	public:
		mem_prop_box(sem_model*, int id);
		void undo();
		void redo();

		int m_iId;
		int change_type;
		diagram_item new_props;

		// item -> item with value before
		QList<diagram_item*> items;

		IType type() { return PROP_BOX; }

	private:
		QHash<diagram_item*, diagram_item*> prev_values;

};

class mem_pos_box : public mem_command {
	public:
		mem_pos_box(sem_model*, int id);
		void undo();
		void redo();

		int m_iId;
		QHash<data_box*, QPointF> prev_values;
		QHash<data_box*, QPointF> next_values;

		IType type() { return POS_BOX; }
};

#endif

