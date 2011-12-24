// Thomas Nagy 2007-2011 GPLV3

#ifndef MEM_COMMAND
#define MEM_COMMAND

#include <QPoint>
#include <QSet>
#include <QList>
#include <QString>

#include "data_item.h"

class sem_model;

// mem_command -> command

class mem_sel;

class mem_command {
	public:
		mem_command(sem_model*);
		sem_model *model;
		virtual void undo() = 0;
		virtual void redo() = 0;
		virtual void apply();
		virtual void redo_dirty();
		virtual void undo_dirty();
		void add();

		bool was_dirty;
		enum IType {DELETE, ADD, LINK, UNLINK, SELECT, MOVE, COLOR, FLAG, EDIT, DATATYPE, TEXT, VARS, PIC, TABLE, SORT,
			ADD_BOX, DEL_BOX, EDIT_BOX};
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

class mem_flag : public mem_command {
	public:
		mem_flag(sem_model*);
		void undo();
		void redo();
		bool add;
		QString flag;
		QMap<int, QList<QString> > prevFlags;
		IType type() { return FLAG; }
};

class mem_edit: public mem_command {
	public:
		mem_edit(sem_model*);
		void undo();
		void redo();
		data_item *sel;
		QString oldSummary;
		QString newSummary;
		IType type() { return EDIT; }
};

class mem_datatype: public mem_command {
	public:
		mem_datatype(sem_model*);
		void undo();
		void redo();
		data_item *sel;
		int oldDataType;
		int newDataType;
		IType type() { return DATATYPE; }
};

class mem_text: public mem_command {
	public:
		mem_text(sem_model*);
		void undo();
		void redo();
		data_item *sel;
		QString oldText;
		QString newText;
		IType type() { return TEXT; }
};

class mem_vars: public mem_command {
	public:
		mem_vars(sem_model*);
		void undo();
		void redo();
		int m_iId;
		QString oldVars;
		QString newVars;
		IType type() { return VARS; }
};

class mem_pic: public mem_command {
	public:
		mem_pic(sem_model*);
		void undo();
		void redo();
		data_item *sel;
		QPixmap oldPix;
		QPixmap newPix;
		IType type() { return PIC; }
};

class mem_table: public mem_command {
	public:
		mem_table(sem_model*);
		void undo();
		void redo();

		int m_iId;
		int oldNRows;
		int oldNCols;
		int newNRows;
		int newNCols;
		QHash<QPair<int, int>, QString> oldData;
		QHash<QPair<int, int>, QString> newData;
		IType type() { return TABLE; }
};

class mem_sort: public mem_command {
	public:
		mem_sort(sem_model*);
		void init(int i_iParent, int i_iChild, int i_iNum);
		void undo();
		void redo();
		int m_iChild;
		int m_iParent;
		QList<QPoint> oldData;
		QList<QPoint> newData;

		IType type() { return SORT; }
};

#endif

