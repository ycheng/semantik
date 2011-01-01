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
	qDebug()<<"apply delete";
	foreach (QPoint p, links) {
		model->unlink_items(p.x(), p.y());
	}
	foreach (data_item* d, items) {
		model->remove_item(d->m_iId);
		qDebug()<<"removing item"<<d->m_iId;
	}
}

void mem_delete::undo() {
	qDebug()<<"revert delete";
	foreach (data_item* d, items) {
		model->m_oItems[d->m_iId] = d;
		model->notify_add_item(d->m_iId);
	}
	foreach (QPoint p, links) {
		model->m_oLinks.append(p);
		//model->notify_link_items(p.x(), p.y());
		/*
		model->link_items(p.x(), p.y());

		hash_params l_oCmd;
		l_oCmd.insert(data_commande, QVariant(cmd_link));
		l_oCmd.insert(data_id, QVariant(i_iChild));
		l_oCmd.insert(data_parent, QVariant(i_iParent));
		emit synchro(l_oCmd);
		*/
	}
}

