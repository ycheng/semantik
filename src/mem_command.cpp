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
	model->m_oRedoStack.clear();
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
		model->unlink_items(p.x(), p.y());
	}
	foreach (data_item* d, items) {
		model->remove_item(d->m_iId);
	}
}

void mem_delete::undo() {
	foreach (data_item* d, items) {
		model->m_oItems[d->m_iId] = d;
		model->notify_add_item(d->m_iId);
	}
	foreach (QPoint p, links) {
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
	model->m_oItems[item->m_iId] = item;
	model->notify_add_item(item->m_iId);
	if (parent) {
		model->m_oLinks.append(QPoint(parent, item->m_iId));
		model->notify_link_items(parent, item->m_iId);
	}
}

void mem_add::undo() {
	if (parent) {
		model->unlink_items(parent, item->m_iId);
	}
	model->remove_item(item->m_iId);
}

///////////////////////////////////////////////////////////////////

mem_link::mem_link(sem_model* mod) : mem_command(mod) {

}

void mem_link::redo() {
	model->m_oLinks.append(QPoint(parent, child));
	model->notify_link_items(parent, child);
}

void mem_link::undo() {
	model->unlink_items(parent, child);
}

///////////////////////////////////////////////////////////////////

mem_unlink::mem_unlink(sem_model* mod) : mem_command(mod) {

}

void mem_unlink::redo() {
	model->unlink_items(parent, child);
}

void mem_unlink::undo() {
	model->m_oLinks.append(QPoint(parent, child));
	model->notify_link_items(parent, child);
}

