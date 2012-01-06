// Thomas Nagy 2007-2012 GPLV3

#include <QPoint>
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

void mem_command::redo_dirty() {
	was_dirty = model->m_bDirty;
	model->set_dirty(true);
}

void mem_command::undo_dirty() {
	model->set_dirty(was_dirty);
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

///////////////////////////////////////////////////////////////////

mem_link::mem_link(sem_model* mod) : mem_command(mod) {

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

mem_unlink::mem_unlink(sem_model* mod) : mem_command(mod) {

}

void mem_unlink::redo() {
	//qDebug()<<"redo mem_link"<<parent<<child;
	Q_ASSERT(model->m_oLinks.contains(QPoint(parent, child)));
	model->m_oLinks.removeAll(QPoint(parent, child));
	model->notify_unlink_items(parent, child);
	redo_dirty();
}

void mem_unlink::undo() {
	//qDebug()<<"undo mem_link"<<parent<<child;
	Q_ASSERT(!model->m_oLinks.contains(QPoint(parent, child)));
	model->m_oLinks.append(QPoint(parent, child));
	model->notify_link_items(parent, child);
	undo_dirty();
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
	redo_dirty();
}

void mem_move::undo() {
	//qDebug()<<"undo mem_move"<<sel;
	for (int i = 0; i < sel.size(); ++i) {
		data_item *it = model->m_oItems[sel[i]];
		it->m_iXX = oldPos[i].x();
		it->m_iYY = oldPos[i].y();
	}
	model->notify_move(sel, oldPos);
	undo_dirty();
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
	redo_dirty();
}

void mem_color::undo() {
	QMap<int, int>::iterator i;
 	for (i = prevColors.begin(); i != prevColors.end(); ++i)
	{
		data_item *t = model->m_oItems[i.key()];
		t->m_iColor = i.value();
		model->notify_repaint(i.key());
	}
	undo_dirty();
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
	redo_dirty();
}

void mem_flag::undo() {
	QMap<int, QList<QString> >::iterator i;
 	for (i = prevFlags.begin(); i != prevFlags.end(); ++i)
	{
		data_item *t = model->m_oItems[i.key()];
		t->m_oFlags = i.value();
		model->notify_flag(i.key());
	}
	undo_dirty();
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
	redo_dirty();
}

void mem_edit::undo() {
	sel->m_sSummary = oldSummary;
	model->notify_edit(sel->m_iId);
	undo_dirty();
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
	redo_dirty();
}

void mem_datatype::undo() {
	sel->m_iDataType = oldDataType;
	model->notify_datatype(sel->m_iId);
	undo_dirty();
}

///////////////////////////////////////////////////////////////////

mem_text::mem_text(sem_model* mod) : mem_command(mod) {
}

void mem_text::redo()
{
	sel->m_sText = newText;
	model->notify_text(sel->m_iId);
	redo_dirty();
}

void mem_text::undo()
{
	sel->m_sText = oldText;
	model->notify_text(sel->m_iId);
	undo_dirty();
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
	redo_dirty();
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
	undo_dirty();
}

///////////////////////////////////////////////////////////////////

mem_pic::mem_pic(sem_model* mod) : mem_command(mod) {
}

void mem_pic::redo()
{
	sel->m_oPix = newPix;
	model->notify_pic(sel->m_iId);
	redo_dirty();
}

void mem_pic::undo()
{
	sel->m_oPix = oldPix;
	model->notify_pic(sel->m_iId);
	undo_dirty();
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
	redo_dirty();
}

void mem_table::undo()
{
	data_item *item = model->m_oItems[m_iId];
	item->m_iNumRows = oldNRows;
	item->m_iNumCols = oldNCols;
	item->m_oTableData = oldData;

	model->notify_table(m_iId);
	undo_dirty();
}

///////////////////////////////////////////////////////////////////

mem_sort::mem_sort(sem_model* mod) : mem_command(mod) {
}

void mem_sort::init(int i_iParent, int i_iChild, int i_iNum)
{
	m_iParent = i_iParent;
	m_iChild = i_iChild;

	int i_iVal1 = -1;
	int i_iVal2 = -1;

	for (int i=0; i < model->m_oLinks.size(); i++)
	{
		QPoint l_oP = model->m_oLinks.at(i);
		if (l_oP.x() == i_iParent)
		{
			oldData.append(l_oP);
			newData.append(l_oP);
		}
	}

	// very old code here
	int l_iIdx = 0;
	for (int i=0; i<newData.size(); i++)
	{
		QPoint l_oP = newData.at(i);
		if (l_oP.x() == i_iParent)
		{
			if (l_iIdx == i_iNum)
				i_iVal1 = i;
			if (l_oP.y() == i_iChild)
				i_iVal2 = i;
			l_iIdx++;
		}

		if (i_iVal1>=0 && i_iVal2>=0)
		{
			if (i_iVal1 > i_iVal2)
			{
				newData.swap(i_iVal1, i_iVal2);
				break;
			}
			else
			{
				int l_iPrev = i_iVal1;
				for (int i=i_iVal1+1; i<i_iVal2+1; i++)
				{
					l_oP = newData.at(i);
					if (l_oP.x() == i_iParent)
						newData.swap(l_iPrev, i);
				}
				break;
			}
		}
	}
}

void mem_sort::redo()
{
	for (int i=0; i < model->m_oLinks.size(); i++)
	{
		QPoint l_oP = model->m_oLinks.at(i);
		if (l_oP.x() == m_iParent)
		{
			model->m_oLinks.removeAt(i);
			i--;
		}
	}
	model->m_oLinks += newData;
	model->notify_sort(m_iParent);
	redo_dirty();
}

void mem_sort::undo()
{
	for (int i=0; i < model->m_oLinks.size(); i++)
	{
		QPoint l_oP = model->m_oLinks.at(i);
		if (l_oP.x() == m_iParent)
		{
			model->m_oLinks.removeAt(i);
			i--;
		}
	}
	model->m_oLinks += oldData;
	model->notify_sort(m_iParent);
	undo_dirty();
}

