// Thomas Nagy 2013-2014 GPLV3

#ifndef MEM_BASE
#define MEM_BASE

#include <QSet>
#include <QPoint>

class data_item;
class sem_mediator;
class mem_command {
	public:
		mem_command(sem_mediator*);
		virtual ~mem_command() {};
		sem_mediator *model;
		virtual void undo() = 0;
		virtual void redo() = 0;
		virtual void apply();
		virtual void redo_dirty();
		virtual void undo_dirty();
		void add();

		bool was_dirty;
		enum IType {DELETE, ADD, LINK, UNLINK, SELECT, MOVE, COLOR, FLAG, EDIT, DATATYPE, TEXT, VARS, PIC, TABLE, SORT,
			ADD_BOX, DEL_BOX, EDIT_BOX, LINK_BOX, UNLINK_BOX, PROP_BOX, POS_BOX, CHANGE_LINK_BOX, SIZE_BOX,
			EDIT_LINK, IMPORT_BOX, SIZE_MATRIX, CHANGE_CLASS_BOX, DIAGRAM_PROPERTIES};
		virtual IType type() = 0;
};

class mem_delete : public mem_command {
	public:
		mem_delete(sem_mediator*);
		void init(QList<int> ids);
		void undo();
		void redo();

		QList<data_item*> items;
		QSet<QPoint> links;

		IType type() { return DELETE; }
};

class mem_sel : public mem_command {
	public:
		mem_sel(sem_mediator*);
		void undo();
		void redo();
		void apply();
		QList<int> unsel;
		QList<int> sel;
		IType type() { return SELECT; }
};

class mem_link : public mem_command {
	public:
		mem_link(sem_mediator*);
		void undo();
		void redo();

		int parent;
		int child;

		IType type() { return LINK; }
};

class mem_add : public mem_command {
	public:
		mem_add(sem_mediator*);
		void init();
		void undo();
		void redo();

		data_item* item;
		int parent;

		mem_sel *sel;

		IType type() { return ADD; }
};

#endif

