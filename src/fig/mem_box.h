// Thomas Nagy 2007-2014 GPLV3

#ifndef MEM_BOX
#define MEM_BOX

#include <QFont>
#include "QHash"
#include "QGraphicsItem"
#include "mem_command.h"

class data_link;

class mem_del_box : public mem_command {
	public:
		mem_del_box(sem_mediator*, int id);
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
		mem_add_box(sem_mediator*, int id, int boxid);
		void undo();
		void redo();

		int m_iId;
		data_item* item;
		data_box* box;

		double m_iXX;
		double m_iYY;

		IType type() { return ADD_BOX; }
};

class mem_edit_box: public mem_command {
	public:
		mem_edit_box(sem_mediator*, int id, int box);
		void undo();
		void redo();

		data_item *item;
		data_box *box;

		int oldHeight;
		int newHeight;

		QString oldText;
		QString newText;
		IType type() { return EDIT_BOX; }
};

class mem_link_box : public mem_command {
	public:
		mem_link_box(sem_mediator*, int id);
		void init(int, int, int, int);
		void undo();
		void redo();

		int m_iId;
		data_link *link;

		IType type() { return LINK_BOX; }
};

class mem_unlink_box : public mem_command {
	public:
		mem_unlink_box(sem_mediator*, int id);
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
		mem_prop_box(sem_mediator*, int id);
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
		mem_pos_box(sem_mediator*, int id);
		void undo();
		void redo();

		int m_iId;
		QHash<data_box*, QPointF> prev_values;
		QHash<data_box*, QPointF> next_values;

		IType type() { return POS_BOX; }
};

class mem_change_link_box : public mem_command {
	public:
		mem_change_link_box(sem_mediator*, int);

		int m_iId;
		data_link prev;
		data_link next;
		data_link *link;

		void undo();
		void redo();
		IType type() { return CHANGE_LINK_BOX; }
};

class mem_size_box : public mem_command {
	public:
		mem_size_box(sem_mediator*, int);
		void undo();
		void redo();
		int m_iId;
		QHash<data_box*, QRect> prev_values;
		QHash<data_box*, QRect> next_values;
		IType type() { return SIZE_BOX; }
};

class mem_edit_link: public mem_command {
	public:
		mem_edit_link(sem_mediator*, int id);
		void undo();
		void redo();

		int m_iId;
		data_link prev;
		data_link next;
		data_link *link;
		QList<diagram_item*> items;

		IType type() { return EDIT_LINK; }
};

class mem_import_box : public mem_command {
	public:
		mem_import_box(sem_mediator*, int id);
		void init(QList<data_box*>, QList<data_link*>);
		void undo();
		void redo();

		int m_iId;
		QList<data_box*> new_items;
		QList<data_link*> new_links;
		QList<data_box*> old_items;
		QList<data_link*> old_links;
		QFont m_iOldFont;
		QFont m_iNewFont;

		IType type() { return IMPORT_BOX; }
};

class mem_matrix : public mem_command {
	public:
		mem_matrix(sem_mediator*, int);
		void init(data_box*);
		void undo();
		void redo();

		int m_iId;
		data_box* m_oBox;
		QList<int> m_oOldRowSizes;
		QList<int> m_oNewRowSizes;
		QList<int> m_oOldColSizes;
		QList<int> m_oNewColSizes;
		int m_iOldWW, m_iNewWW, m_iOldHH, m_iNewHH;

		IType type() { return SIZE_MATRIX; }
};

class mem_class : public mem_command {
	public:
		mem_class(sem_mediator*, int);
		void init(data_box*);
		void undo();
		void redo();

		int m_iId;
		int m_iBoxId;

		data_box m_oOldBox;
		data_box m_oNewBox;

		IType type() { return CHANGE_CLASS_BOX; }
};

class mem_diagram_properties : public mem_command {
	public:
		mem_diagram_properties(sem_mediator*, int);
		void undo();
		void redo();

		int m_iId;
		QFont m_oOldFont;
		QFont m_oNewFont;

		IType type() { return DIAGRAM_PROPERTIES; }
};

#endif

