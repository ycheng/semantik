// Thomas Nagy 2013 GPLV3

#include <QPoint>
#include <QDebug>

#include "data_item.h"
#include "mem_command.h"
#include "sem_mediator.h"


#include "mem_base.h"

mem_command::mem_command(sem_mediator* mod) {
	model = mod;
}

void mem_command::apply() {
	while (!model->m_oRedoStack.isEmpty())
		delete model->m_oRedoStack.pop();
	redo();
	model->m_oUndoStack.push(this);
	model->check_undo(true);
}

void mem_command::add() {
	while (!model->m_oRedoStack.isEmpty())
		delete model->m_oRedoStack.pop();
	model->m_oUndoStack.push(this);
	model->check_undo(true);
}

void mem_command::redo_dirty() {
	was_dirty = model->m_bDirty;
	model->set_dirty(true);
}

void mem_command::undo_dirty() {
	model->set_dirty(was_dirty);
}


