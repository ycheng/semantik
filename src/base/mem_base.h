// Thomas Nagy 2013 GPLV3

#ifndef MEM_BASE
#define MEM_BASE

class sem_mediator;
class mem_sel;
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
			ADD_BOX, DEL_BOX, EDIT_BOX, LINK_BOX, UNLINK_BOX, PROP_BOX, POS_BOX, CHANGE_LINK_BOX, SIZE_BOX, EDIT_LINK, IMPORT_BOX};
		virtual IType type() = 0;
};

#endif

