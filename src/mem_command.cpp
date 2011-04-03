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
	model->check_undo(true);
}

void mem_command::add() {
	while (!model->m_oRedoStack.isEmpty())
		delete model->m_oRedoStack.pop();
	model->m_oUndoStack.push(this);
	model->check_undo(true);
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
}

///////////////////////////////////////////////////////////////////

mem_link::mem_link(sem_model* mod) : mem_command(mod) {

}

void mem_link::redo() {
	//qDebug()<<"redo mem_link"<<parent<<child;
	Q_ASSERT(!model->m_oLinks.contains(QPoint(parent, child)));
	model->m_oLinks.append(QPoint(parent, child));
	model->notify_link_items(parent, child);
}

void mem_link::undo() {
	//qDebug()<<"undo mem_link"<<parent<<child;
	Q_ASSERT(model->m_oLinks.contains(QPoint(parent, child)));
	model->m_oLinks.removeAll(QPoint(parent, child));
	model->notify_unlink_items(parent, child);
}

///////////////////////////////////////////////////////////////////

mem_unlink::mem_unlink(sem_model* mod) : mem_command(mod) {

}

void mem_unlink::redo() {
	//qDebug()<<"redo mem_link"<<parent<<child;
	Q_ASSERT(model->m_oLinks.contains(QPoint(parent, child)));
	model->m_oLinks.removeAll(QPoint(parent, child));
	model->notify_unlink_items(parent, child);
}

void mem_unlink::undo() {
	//qDebug()<<"undo mem_link"<<parent<<child;
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

mem_move::mem_move(sem_model* mod) : mem_command(mod) {
	foreach (data_item* t, model->m_oItems.values()) {
		if (t->m_bSelected) {
			sel.append(t->m_iId);
		}
	}
}

void mem_move::redo() {
	//qDebug()<<"redo mem_move"<<sel;
	for (int i = 0; i < sel.size(); ++i) {
		data_item *it = model->m_oItems[sel[i]];
		it->m_iXX = newPos[i].x();
		it->m_iYY = newPos[i].y();
	}
	model->notify_move(sel, newPos);
}

void mem_move::undo() {
	//qDebug()<<"undo mem_move"<<sel;
	for (int i = 0; i < sel.size(); ++i) {
		data_item *it = model->m_oItems[sel[i]];
		it->m_iXX = oldPos[i].x();
		it->m_iYY = oldPos[i].y();
	}
	model->notify_move(sel, oldPos);
}

///////////////////////////////////////////////////////////////////

mem_color::mem_color(sem_model* mod) : mem_command(mod) {
	foreach (data_item* t, model->m_oItems.values()) {
		if (t->m_bSelected) {
			prevColors[t->m_iId] = t->m_iColor;
		}
	}
}

void mem_color::redo() {
	QMap<int, int>::iterator i;
 	for (i = prevColors.begin(); i != prevColors.end(); ++i)
	{
		data_item *t = model->m_oItems[i.key()];
		t->m_iColor = newColor;
		model->notify_repaint(i.key());
	}
}

void mem_color::undo() {
	QMap<int, int>::iterator i;
 	for (i = prevColors.begin(); i != prevColors.end(); ++i)
	{
		data_item *t = model->m_oItems[i.key()];
		t->m_iColor = i.value();
		model->notify_repaint(i.key());
	}
}

///////////////////////////////////////////////////////////////////

mem_flag::mem_flag(sem_model* mod) : mem_command(mod) {
	add = true;
	foreach (data_item* t, model->m_oItems.values()) {
		if (t->m_bSelected) {
			prevFlags[t->m_iId] = t->m_oFlags;
		}
	}
}

void mem_flag::redo() {
	QMap<int, QList<QString> >::iterator i;
 	for (i = prevFlags.begin(); i != prevFlags.end(); ++i)
	{
		data_item *t = model->m_oItems[i.key()];
		t->m_oFlags = QList<QString>(i.value());
		if (add) {
			if (!t->m_oFlags.contains(flag)) {
				t->m_oFlags.append(flag);
			}
		} else {
			t->m_oFlags.removeAll(flag);
		}
		model->notify_flag(i.key());
	}
}

void mem_flag::undo() {
	QMap<int, QList<QString> >::iterator i;
 	for (i = prevFlags.begin(); i != prevFlags.end(); ++i)
	{
		data_item *t = model->m_oItems[i.key()];
		t->m_oFlags = i.value();
		model->notify_flag(i.key());
	}
}

///////////////////////////////////////////////////////////////////

mem_edit::mem_edit(sem_model* mod) : mem_command(mod) {
	foreach (data_item* t, model->m_oItems.values()) {
		if (t->m_bSelected) {
			sel = t;
		}
	}
	oldSummary = sel->m_sSummary;
}

void mem_edit::redo() {
	sel->m_sSummary = newSummary;
	model->notify_edit(sel->m_iId);
}

void mem_edit::undo() {
	sel->m_sSummary = oldSummary;
	model->notify_edit(sel->m_iId);
}

///////////////////////////////////////////////////////////////////

mem_datatype::mem_datatype(sem_model* mod) : mem_command(mod) {
	sel = NULL;
	foreach (data_item* t, model->m_oItems.values()) {
		if (t->m_bSelected) {
			sel = t;
			oldDataType = sel->m_iDataType;
			break;
		}
	}
}

void mem_datatype::redo() {
	sel->m_iDataType = newDataType;
	model->notify_datatype(sel->m_iId);
}

void mem_datatype::undo() {
	sel->m_iDataType = oldDataType;
	model->notify_datatype(sel->m_iId);
}

///////////////////////////////////////////////////////////////////

mem_text::mem_text(sem_model* mod) : mem_command(mod) {
}

void mem_text::redo()
{
	sel->m_sText = newText;
	model->notify_text(sel->m_iId);
}

void mem_text::undo()
{
	sel->m_sText = oldText;
	model->notify_text(sel->m_iId);
}

///////////////////////////////////////////////////////////////////

mem_vars::mem_vars(sem_model* mod) : mem_command(mod) {
}

void mem_vars::redo()
{
	if (m_iId == NO_ITEM)
	{
		model->m_sHints = newVars;
	}
	else
	{
		data_item *item = model->m_oItems[m_iId];
		item->m_sHints = newVars;
	}
	model->notify_vars(m_iId);
}

void mem_vars::undo()
{
	if (m_iId == NO_ITEM)
	{
		model->m_sHints = oldVars;
	}
	else
	{
		data_item *item = model->m_oItems[m_iId];
		item->m_sHints = oldVars;
	}
	model->notify_vars(m_iId);
}

///////////////////////////////////////////////////////////////////

mem_pic::mem_pic(sem_model* mod) : mem_command(mod) {
}

void mem_pic::redo()
{
	sel->m_oPix = newPix;
	model->notify_pic(sel->m_iId);
}

void mem_pic::undo()
{
	sel->m_oPix = oldPix;
	model->notify_pic(sel->m_iId);
}

///////////////////////////////////////////////////////////////////

mem_table::mem_table(sem_model* mod) : mem_command(mod) {
}

void mem_table::redo()
{
	data_item *item = model->m_oItems[m_iId];
	item->m_iNumRows = newNRows;
	item->m_iNumCols = newNCols;
	item->m_oTableData = newData;

	model->notify_table(m_iId);
}

void mem_table::undo()
{
	data_item *item = model->m_oItems[m_iId];
	item->m_iNumRows = oldNRows;
	item->m_iNumCols = oldNCols;
	item->m_oTableData = oldData;

	model->notify_table(m_iId);
}

