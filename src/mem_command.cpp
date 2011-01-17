// Thomas Nagy 2007-2011 GPLV3

#include <QDebug>
#include "data_item.h"
#include "mem_command.h"
#include "sem_model.h"

inline uint qHash(const QPoint &p) {
	return p.x() + p.y();
}

mem_command::mem_command(sem_model* mod) {
	model = mod;
}

void mem_command::apply() {
	while (!model->m_oRedoStack.isEmpty())
		delete model->m_oRedoStack.pop();
	redo();
	model->m_oUndoStack.push(this);
}

///////////////////////////////////////////////////////////////////

mem_delete::mem_delete(sem_model* mod) : mem_command(mod) {

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
}

///////////////////////////////////////////////////////////////////

mem_add::mem_add(sem_model* mod) : mem_command(mod) {
	parent = NO_ITEM;
}

void mem_add::init() {
	item = new data_item(model, model->next_seq());
}

void mem_add::redo() {
	Q_ASSERT(!model->m_oItems.contains(item->m_iId));
	model->m_oItems[item->m_iId] = item;
	model->notify_add_item(item->m_iId);
	if (parent) {
		Q_ASSERT(!model->m_oLinks.contains(QPoint(parent, item->m_iId)));
		model->m_oLinks.append(QPoint(parent, item->m_iId));
		model->notify_link_items(parent, item->m_iId);
	}
}

void mem_add::undo() {
	if (parent) {
		Q_ASSERT(model->m_oLinks.contains(QPoint(parent, item->m_iId)));
		model->m_oLinks.removeAll(QPoint(parent, item->m_iId));
		model->notify_unlink_items(parent, item->m_iId);
	}
	Q_ASSERT(model->m_oItems.contains(item->m_iId));
	model->notify_delete_item(item->m_iId);
	model->m_oItems.remove(item->m_iId);
}

///////////////////////////////////////////////////////////////////

mem_link::mem_link(sem_model* mod) : mem_command(mod) {

}

void mem_link::redo() {
	Q_ASSERT(!model->m_oLinks.contains(QPoint(parent, child)));
	model->m_oLinks.append(QPoint(parent, child));
	model->notify_link_items(parent, child);
}

void mem_link::undo() {
	Q_ASSERT(model->m_oLinks.contains(QPoint(parent, child)));
	model->m_oLinks.removeAll(QPoint(parent, child));
	model->notify_unlink_items(parent, child);
}

///////////////////////////////////////////////////////////////////

mem_unlink::mem_unlink(sem_model* mod) : mem_command(mod) {

}

void mem_unlink::redo() {
	Q_ASSERT(model->m_oLinks.contains(QPoint(parent, child)));
	model->m_oLinks.removeAll(QPoint(parent, child));
	model->notify_unlink_items(parent, child);
}

void mem_unlink::undo() {
	Q_ASSERT(!model->m_oLinks.contains(QPoint(parent, child)));
	model->m_oLinks.append(QPoint(parent, child));
	model->notify_link_items(parent, child);
}

///////////////////////////////////////////////////////////////////

mem_sel::mem_sel(sem_model* mod) : mem_command(mod) {
	foreach (data_item* t, model->m_oItems.values()) {
		if (t->m_bSelected) {
			unsel.append(t->m_iId);
		}
	}
}

void mem_sel::apply() {
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
}

void mem_sel::redo() {
	foreach (int k, unsel) {
		model->m_oItems[k]->m_bSelected = false;
	}
	foreach (int k, sel) {
		model->m_oItems[k]->m_bSelected = true;
	}
	model->notify_select(unsel, sel);
}

void mem_sel::undo() {
	foreach (int k, sel) {
		model->m_oItems[k]->m_bSelected = false;
	}
	foreach (int k, unsel) {
		model->m_oItems[k]->m_bSelected = true;
	}
	model->notify_select(sel, unsel);
}

///////////////////////////////////////////////////////////////////

mem_move::mem_move(sem_model* mod) : mem_command(mod) {
	foreach (data_item* t, model->m_oItems.values()) {
		if (t->m_bSelected) {
			sel.append(t->m_iId);
		}
	}
}

void mem_move::redo() {
	model->notify_move(sel, newPos);
}

void mem_move::undo() {
	model->notify_move(sel, oldPos);
}

