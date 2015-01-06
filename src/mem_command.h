// Thomas Nagy 2007-2015 GPLV3

#ifndef MEM_COMMAND
#define MEM_COMMAND

#include "mem_base.h"

#include <QPoint>
#include <QSet>
#include <QList>
#include <QString>

#include "data_item.h"


class mem_unlink : public mem_command {
	public:
		mem_unlink(sem_mediator*);
		void undo();
		void redo();

		int parent;
		int child;
		IType type() { return UNLINK; }
};

class mem_move : public mem_command {
	public:
		mem_move(sem_mediator*);
		void undo();
		void redo();
		QList<int> sel;
		QList<QPointF> oldPos;
		QList<QPointF> newPos;
		IType type() { return MOVE; }
};

class mem_color : public mem_command {
	public:
		mem_color(sem_mediator*);
		void undo();
		void redo();
		QMap<int, int> prevColors;
		int newColor;
		IType type() { return COLOR; }
};

class mem_flag : public mem_command {
	public:
		mem_flag(sem_mediator*);
		void undo();
		void redo();
		bool add;
		QString flag;
		QMap<int, QList<QString> > prevFlags;
		IType type() { return FLAG; }
};

class mem_edit: public mem_command {
	public:
		mem_edit(sem_mediator*);
		void undo();
		void redo();
		data_item *sel;
		QString oldSummary;
		QString newSummary;
		IType type() { return EDIT; }
};

class mem_datatype: public mem_command {
	public:
		mem_datatype(sem_mediator*);
		void undo();
		void redo();
		data_item *sel;
		int oldDataType;
		int newDataType;
		IType type() { return DATATYPE; }
};

class mem_text: public mem_command {
	public:
		mem_text(sem_mediator*);
		void undo();
		void redo();
		data_item *sel;
		QString oldText;
		QString newText;
		IType type() { return TEXT; }
};

class mem_vars: public mem_command {
	public:
		mem_vars(sem_mediator*);
		void undo();
		void redo();
		int m_iId;
		QString oldVars;
		QString newVars;
		IType type() { return VARS; }
};

class mem_pic: public mem_command {
	public:
		mem_pic(sem_mediator*);
		void undo();
		void redo();
		data_item *sel;
		int m_iOldId;
		int m_iNewId;
		IType type() { return PIC; }
};

class mem_table: public mem_command {
	public:
		mem_table(sem_mediator*);
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
		mem_sort(sem_mediator*);
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

