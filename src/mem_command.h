// Thomas Nagy 2007-2011 GPLV3

#ifndef MEM_COMMAND
#define MEM_COMMAND

#include <QPoint>
#include <QSet>
#include <QList>

class data_item;
class sem_model;

class mem_sel;

class mem_command {
	public:
		mem_command(sem_model*);
		sem_model *model;
		virtual void undo() = 0;
		virtual void redo() = 0;
		virtual void apply();

		enum IType {DELETE, ADD, LINK, UNLINK, SELECT, MOVE, COLOR};
		virtual IType type() = 0;
};

class mem_delete : public mem_command {
	public:
		mem_delete(sem_model*);
		void init(QList<int> ids);
		void undo();
		void redo();

		QList<data_item*> items;
		QSet<QPoint> links;

		IType type() { return DELETE; }
};

class mem_add : public mem_command {
	public:
		mem_add(sem_model*);
		void init();
		void undo();
		void redo();

		data_item* item;
		int parent;

		mem_sel *sel;

		IType type() { return ADD; }
};

class mem_link : public mem_command {
	public:
		mem_link(sem_model*);
		void undo();
		void redo();

		int parent;
		int child;

		IType type() { return LINK; }
};

class mem_unlink : public mem_command {
	public:
		mem_unlink(sem_model*);
		void undo();
		void redo();

		int parent;
		int child;
		IType type() { return UNLINK; }
};

class mem_sel : public mem_command {
	public:
		mem_sel(sem_model*);
		void undo();
		void redo();
		void apply();
		QList<int> unsel;
		QList<int> sel;
		IType type() { return SELECT; }
};

class mem_move : public mem_command {
	public:
		mem_move(sem_model*);
		void undo();
		void redo();
		QList<int> sel;
		QList<QPointF> oldPos;
		QList<QPointF> newPos;
		IType type() { return MOVE; }
};

class mem_color : public mem_command {
	public:
		mem_color(sem_model*);
		void undo();
		void redo();
		QMap<int, int> prevColors;
		int newColor;
		IType type() { return COLOR; }
};

#endif

