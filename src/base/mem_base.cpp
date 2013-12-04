// Thomas Nagy 2013 GPLV3

#include <QPoint>
#include <QDebug>

#include "data_item.h"
#include "mem_command.h"
#include "sem_mediator.h"


#include "mem_base.h"

inline uint qHash(const QPoint &p) {
	return p.x() + p.y();
}


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

///////////////////////////////////////////////////////////////////

mem_delete::mem_delete(sem_mediator* mod) : mem_command(mod) {

}

void mem_delete::init(QList<int> lst) {
	foreach (int id, lst) {
		items.append(*model + id);
		for (int i = 0; i < model->m_oLinks.size(); i++) {
			QPoint p = model->m_oLinks.at(i);
			if (p.x() == id || p.y() == id) {
				links.insert(p);
			}
		}
	}
}

void mem_delete::redo() {
	foreach (QPoint p, links) {
		Q_ASSERT(model->m_oLinks.contains(p));
		model->m_oLinks.removeAll(p);
		model->notify_unlink_items(p.x(), p.y());
	}
	foreach (data_item* d, items) {
		Q_ASSERT(model->m_oItems.contains(d->m_iId));
		model->notify_delete_item(d->m_iId);
		model->m_oItems.remove(d->m_iId);
	}
	redo_dirty();
}

void mem_delete::undo() {
	foreach (data_item* d, items) {
		Q_ASSERT(!model->m_oItems.contains(d->m_iId));
		model->m_oItems[d->m_iId] = d;
		model->notify_add_item(d->m_iId);
	}
	foreach (QPoint p, links) {
		Q_ASSERT(!model->m_oLinks.contains(p));
		model->m_oLinks.append(p);
		model->notify_link_items(p.x(), p.y());
	}
	undo_dirty();
}

///////////////////////////////////////////////////////////////////

mem_link::mem_link(sem_mediator* mod) : mem_command(mod) {

}

void mem_link::redo() {
	//qDebug()<<"redo mem_link"<<parent<<child;
	Q_ASSERT(!model->m_oLinks.contains(QPoint(parent, child)));
	model->m_oLinks.append(QPoint(parent, child));
	model->notify_link_items(parent, child);
	redo_dirty();
}

void mem_link::undo() {
	//qDebug()<<"undo mem_link"<<parent<<child;
	Q_ASSERT(model->m_oLinks.contains(QPoint(parent, child)));
	model->m_oLinks.removeAll(QPoint(parent, child));
	model->notify_unlink_items(parent, child);
	undo_dirty();
}

///////////////////////////////////////////////////////////////////

mem_sel::mem_sel(sem_mediator* mod) : mem_command(mod) {
	foreach (data_item* t, model->m_oItems.values()) {
		if (t->m_bSelected) {
			unsel.append(t->m_iId);
		}
	}
}

void mem_sel::apply() {
	//qDebug()<<"apply sel begin"<<model->m_oUndoStack.size()<<model->m_oRedoStack.size();

	while (!model->m_oRedoStack.isEmpty())
		delete model->m_oRedoStack.pop();

	// merge a previous selection if possible
	while (!model->m_oUndoStack.empty()) {
		mem_command *me = model->m_oUndoStack.pop();
		if (me->type() == SELECT) {
			mem_sel *sal = (mem_sel*) me;
			foreach (int i, sal->sel) {
				if (!sel.contains(i) && !unsel.contains(i))
					sel.append(i);
			}
			foreach (int i, sal->unsel) {
				if (!sel.contains(i) && !unsel.contains(i))
					unsel.append(i);
			}
			delete sal;
		} else {
			model->m_oUndoStack.push(me);
			break;
		}
	}

	foreach (int k, sel) {
		unsel.removeAll(k);
	}

	// normal processing
	redo();
	model->m_oUndoStack.push(this);
	//qDebug()<<"apply sel end"<<model->m_oUndoStack.size()<<model->m_oRedoStack.size();
	model->check_undo(true);
}

void mem_sel::redo() {
	//qDebug()<<"redo mem_sel"<<sel<<unsel;
	foreach (int k, unsel) {
		model->m_oItems[k]->m_bSelected = false;
	}
	foreach (int k, sel) {
		model->m_oItems[k]->m_bSelected = true;
	}
	model->notify_select(unsel, sel);
}

void mem_sel::undo() {
	//qDebug()<<"undo mem_sel"<<sel<<unsel;
	foreach (int k, sel) {
		model->m_oItems[k]->m_bSelected = false;
	}
	foreach (int k, unsel) {
		model->m_oItems[k]->m_bSelected = true;
	}
	model->notify_select(sel, unsel);
}

///////////////////////////////////////////////////////////////////

mem_add::mem_add(sem_mediator* mod) : mem_command(mod) {
	parent = NO_ITEM;
}

void mem_add::init() {
	item = new data_item(model, model->next_seq());
	sel = new mem_sel(model);
}

void mem_add::redo() {
	//qDebug()<<"redo mem_add"<<item->m_iId;
	Q_ASSERT(!model->m_oItems.contains(item->m_iId));
	model->m_oItems[item->m_iId] = item;
	model->notify_add_item(item->m_iId);
	if (parent) {
		Q_ASSERT(!model->m_oLinks.contains(QPoint(parent, item->m_iId)));
		model->m_oLinks.append(QPoint(parent, item->m_iId));
		model->notify_link_items(parent, item->m_iId);
	}

	if (sel->sel.size() != 1) {
		sel->sel.append(item->m_iId);
	}
	sel->redo();
	redo_dirty();
}

void mem_add::undo() {
	//qDebug()<<"undo mem_add"<<item->m_iId;
	sel->undo();

	if (parent) {
		Q_ASSERT(model->m_oLinks.contains(QPoint(parent, item->m_iId)));
		model->m_oLinks.removeAll(QPoint(parent, item->m_iId));
		model->notify_unlink_items(parent, item->m_iId);
	}
	Q_ASSERT(model->m_oItems.contains(item->m_iId));
	model->notify_delete_item(item->m_iId);
	model->m_oItems.remove(item->m_iId);
	undo_dirty();
}


