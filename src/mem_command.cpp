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

