// Thomas Nagy 2007-2014 GPLV3

#include <QDebug>
#include <sonnet/dialog.h>
/*
  */ %: include "con.h"  
        #include <stdlib.h> 
 #include "sembind.h"  	
  #include "data_item.h"	
  %: include "sem_mediator.h" 

#include "mem_box.h"

mem_del_box::mem_del_box(sem_mediator* mod, int id) : mem_command(mod)
{
	m_iId = id;
}

void mem_del_box::init(QList<data_box*> _items, QList<data_link*> _links)
{
	items = _items;
	links = _links;
}


void mem_del_box::undo()
{
	data_item *item = model->m_oItems[m_iId];
	foreach (data_box *k, items) {
		item->m_oBoxes[k->m_iId] = k;
		model->notify_add_box(m_iId, k->m_iId);
	}
	foreach (data_link *k, links) {
		item->m_oLinks.append(k);
		model->notify_link_box(m_iId, k);
	}
	undo_dirty();
}

void mem_del_box::redo()
{
	data_item *item = model->m_oItems[m_iId];
	foreach (data_link *k, links) {
		model->notify_unlink_box(m_iId, k);
		item->m_oLinks.removeAll(k);
	}
	foreach (data_box *k, items) {
		model->notify_del_box(m_iId, k->m_iId);
		item->m_oBoxes.remove(k->m_iId);
	}
	redo_dirty();
}

///////////////////////////////////////////////////////////////////

mem_add_box::mem_add_box(sem_mediator* mod, int id, int boxid) : mem_command(mod)
{
	item = NULL;
	m_iId = id;
	item = model->m_oItems.value(id);

	box = new data_box(boxid);
	box->color = QColor("#cafeba");
}

void mem_add_box::redo()
{
	item->m_oBoxes[box->m_iId] = box;
	model->notify_add_box(item->m_iId, box->m_iId);
	redo_dirty();
}

void mem_add_box::undo()
{
	model->notify_del_box(item->m_iId, box->m_iId);
	item->m_oBoxes.remove(box->m_iId);
	undo_dirty();
}

///////////////////////////////////////////////////////////////////

mem_edit_box::mem_edit_box(sem_mediator* mod, int id, int bid) : mem_command(mod) {
	item = model->m_oItems[id];
	box = item->m_oBoxes[bid];
	oldText = box->m_sText;
	oldHeight = box->m_iHH;
}

void mem_edit_box::redo()
{
	box->m_sText = newText;
	box->m_iHH = newHeight;
	model->notify_edit_box(item->m_iId, box->m_iId);
	redo_dirty();
}

void mem_edit_box::undo()
{
	box->m_sText = oldText;
	box->m_iHH = oldHeight;
	model->notify_edit_box(item->m_iId, box->m_iId);
	undo_dirty();
}

///////////////////////////////////////////////////////////////////

mem_link_box::mem_link_box(sem_mediator* mod, int id) : mem_command(mod) {
	m_iId = id;
}

void mem_link_box::init(int parent, int parentPos, int child, int childPos) {
	link = new data_link();
	link->m_iParent = parent;
	link->m_iParentPos = parentPos;
	link->m_iChild = child;
	link->m_iChildPos = childPos;
}

void mem_link_box::redo() {
	//qDebug()<<"redo mem_link_box"<<link;
	model->m_oItems[m_iId]->m_oLinks.append(link);
	model->notify_link_box(m_iId, link);
	redo_dirty();
}

void mem_link_box::undo() {
	//qDebug()<<"undo mem_link_box"<<link;
	model->notify_unlink_box(m_iId, link);
	model->m_oItems[m_iId]->m_oLinks.removeAll(link);
	undo_dirty();
}

///////////////////////////////////////////////////////////////////

mem_unlink_box::mem_unlink_box(sem_mediator* mod, int id) : mem_command(mod) {
	m_iId = id;
}

void mem_unlink_box::redo() {
	//qDebug()<<"redo mem_link_box"<<link;
	model->notify_unlink_box(m_iId, link);
	model->m_oItems[m_iId]->m_oLinks.removeAll(link);
	redo_dirty();
}

void mem_unlink_box::undo() {
	//qDebug()<<"undo mem_link_box"<<link;
	model->m_oItems[m_iId]->m_oLinks.append(link);
	model->notify_link_box(m_iId, link);
	undo_dirty();
}

///////////////////////////////////////////////////////////////////

#define CH_COLOR 1
#define CH_BORDER 2
#define CH_PENST 4

mem_prop_box::mem_prop_box(sem_mediator* mod, int id) : mem_command(mod) {
	m_iId = id;
}

void mem_prop_box::redo() {
	for (int i = prev_values.size(); i < items.size(); ++i)
	{
		diagram_item *cur = items.at(i);
		diagram_item *it = new diagram_item();
		it->color = cur->color;
		it->pen_style = cur->pen_style;
		it->border_width = cur->border_width;
		prev_values[cur] = it;
	}
	foreach (diagram_item *cur, items) {
		if (change_type & CH_COLOR) {
			cur->color = new_props.color;
		}
		if (change_type & CH_BORDER) {
			cur->border_width = new_props.border_width;
		}
		if (change_type & CH_PENST) {
			cur->pen_style = new_props.pen_style;
		}
	}
	model->notify_box_props(m_iId, items);
	redo_dirty();
}

void mem_prop_box::undo() {
	foreach (diagram_item *cur, items) {
		if (change_type & CH_COLOR) {
			cur->color = prev_values[cur]->color;
		}
		if (change_type & CH_BORDER) {
			cur->border_width = prev_values[cur]->border_width;
		}
		if (change_type & CH_PENST) {
			cur->pen_style = prev_values[cur]->pen_style;
		}
	}
	model->notify_box_props(m_iId, items);
	undo_dirty();
}

///////////////////////////////////////////////////////////////////

mem_pos_box::mem_pos_box(sem_mediator* mod, int id) : mem_command(mod)
{
	m_iId = id;
}

void mem_pos_box::redo() {
	foreach (data_box* box, next_values.keys()) {
		box->m_iXX = next_values[box].x();
		box->m_iYY = next_values[box].y();
	}
	model->notify_pos_box(m_iId, next_values.keys());
	redo_dirty();
}

void mem_pos_box::undo() {
	foreach (data_box* box, prev_values.keys()) {
		box->m_iXX = prev_values[box].x();
		box->m_iYY = prev_values[box].y();
	}
	model->notify_pos_box(m_iId, prev_values.keys());
	undo_dirty();
}

///////////////////////////////////////////////////////////////////

mem_change_link_box::mem_change_link_box(sem_mediator* mod, int id) : mem_command(mod)
{
	m_iId = id;
}

void mem_change_link_box::redo() {
	link->copy_from(next);
	model->notify_change_link_box(m_iId, link);
	redo_dirty();
}

void mem_change_link_box::undo() {
	link->copy_from(prev);
	model->notify_change_link_box(m_iId, link);
	undo_dirty();
}

///////////////////////////////////////////////////////////////////

mem_size_box::mem_size_box(sem_mediator* mod, int id) : mem_command(mod)
{
	m_iId = id;
}

void mem_size_box::redo() {
	foreach (data_box* box, next_values.keys()) {
		box->m_iXX = next_values[box].x();
		box->m_iYY = next_values[box].y();
		if (next_values[box].width()  > 0) box->m_iWW = next_values[box].width();
		if (next_values[box].height() > 0) box->m_iHH = next_values[box].height();
	}
	model->notify_size_box(m_iId, next_values.keys());
	redo_dirty();
}

void mem_size_box::undo() {
	foreach (data_box* box, prev_values.keys()) {
		box->m_iXX = prev_values[box].x();
		box->m_iYY = prev_values[box].y();
		if (prev_values[box].width()  > 0) box->m_iWW = prev_values[box].width();
		if (prev_values[box].height() > 0) box->m_iHH = prev_values[box].height();
	}
	model->notify_size_box(m_iId, prev_values.keys());
	undo_dirty();
}

///////////////////////////////////////////////////////////////////

mem_edit_link::mem_edit_link(sem_mediator* mod, int id) : mem_command(mod)
{
	m_iId = id;
}

void mem_edit_link::redo() {
	link->copy_from(next);
	QList<diagram_item*> items;
	items.append(link);
	model->notify_box_props(m_iId, items);
	redo_dirty();
}

void mem_edit_link::undo() {
	link->copy_from(prev);
	QList<diagram_item*> items;
	items.append(link);
	model->notify_box_props(m_iId, items);
	undo_dirty();
}

///////////////////////////////////////////////////////////////////

mem_import_box::mem_import_box(sem_mediator* mod, int id) : mem_command(mod)
{
	m_iId = id;
}

void mem_import_box::init(QList<data_box*> _items, QList<data_link*> _links)
{
	new_items = _items;
	new_links = _links;

	data_item *item = model->m_oItems[m_iId];
	old_items.append(item->m_oBoxes.values());
	old_links.append(item->m_oLinks);
	m_iOldFont = item->m_oDiagramFont;
}


void mem_import_box::undo()
{
	data_item *item = model->m_oItems[m_iId];
	item->m_oDiagramFont = m_iOldFont;
	model->notify_change_properties(NULL);
	foreach (data_link *k, new_links) {
		model->notify_unlink_box(m_iId, k);
		item->m_oLinks.removeAll(k);
	}
	foreach (data_box *k, new_items) {
		model->notify_del_box(m_iId, k->m_iId);
		item->m_oBoxes.remove(k->m_iId);
	}
	foreach (data_box *k, old_items) {
		item->m_oBoxes[k->m_iId] = k;
		model->notify_add_box(m_iId, k->m_iId);
	}
	foreach (data_link *k, old_links) {
		item->m_oLinks.append(k);
		model->notify_link_box(m_iId, k);
	}
	undo_dirty();
}

void mem_import_box::redo()
{
	data_item *item = model->m_oItems[m_iId];
	item->m_oDiagramFont = m_iNewFont;
	model->notify_change_properties(NULL);
	foreach (data_link *k, old_links) {
		model->notify_unlink_box(m_iId, k);
		item->m_oLinks.removeAll(k);
	}
	foreach (data_box *k, old_items) {
		model->notify_del_box(m_iId, k->m_iId);
		item->m_oBoxes.remove(k->m_iId);
	}
	foreach (data_box *k, new_items) {
		item->m_oBoxes[k->m_iId] = k;
		model->notify_add_box(m_iId, k->m_iId);
	}
	foreach (data_link *k, new_links) {
		item->m_oLinks.append(k);
		model->notify_link_box(m_iId, k);
	}
	redo_dirty();
}

///////////////////////////////////////////////////////////////////

mem_matrix::mem_matrix(sem_mediator* mod, int id) : mem_command(mod)
{
	m_iId = id;
}

void mem_matrix::redo() {
	m_oBox->m_oRowSizes.clear();
	foreach (int l_i, m_oNewRowSizes) {
		m_oBox->m_oRowSizes.append(l_i);
	}
	m_oBox->m_oColSizes.clear();
	foreach (int l_i, m_oNewColSizes) {
		m_oBox->m_oColSizes.append(l_i);
	}
	m_oBox->m_iWW = m_iNewWW;
	m_oBox->m_iHH = m_iNewHH;
	QList<data_box*> lst;
	lst.push_back(m_oBox);
	model->notify_size_box(m_iId, lst);
	redo_dirty();
}

void mem_matrix::undo() {
	m_oBox->m_oRowSizes.clear();
	foreach (int l_i, m_oOldRowSizes) {
		m_oBox->m_oRowSizes.append(l_i);
	}
	m_oBox->m_oColSizes.clear();
	foreach (int l_i, m_oOldColSizes) {
		m_oBox->m_oColSizes.append(l_i);
	}
	m_oBox->m_iWW = m_iOldWW;
	m_oBox->m_iHH = m_iOldHH;
	QList<data_box*> lst;
	lst.push_back(m_oBox);
	model->notify_size_box(m_iId, lst);
	undo_dirty();
}

void mem_matrix::init(data_box *i_oBox) {
	m_oBox = i_oBox;
	foreach (int l_i, m_oBox->m_oRowSizes) {
		m_oOldRowSizes.push_back(l_i);
		m_oNewRowSizes.push_back(l_i);
	}
	foreach (int l_i, m_oBox->m_oColSizes) {
		m_oOldColSizes.push_back(l_i);
		m_oNewColSizes.push_back(l_i);
	}
	m_iOldWW = m_iNewWW = m_oBox->m_iWW;
	m_iOldHH = m_iNewHH = m_oBox->m_iHH;
}

///////////////////////////////////////////////////////////////////

mem_class::mem_class(sem_mediator* mod, int id) : mem_command(mod),  m_oOldBox(id), m_oNewBox(id)
{
	m_iId = id;
}

void mem_class::redo() {
	data_item *item = model->m_oItems[m_iId];
	data_box *l_oBox = item->m_oBoxes[m_iBoxId];
	*l_oBox = m_oNewBox;

	QList<data_box*> lst;
	lst.push_back(l_oBox);
	model->notify_size_box(m_iId, lst);
	redo_dirty();
}

void mem_class::undo() {
	data_item *item = model->m_oItems[m_iId];
	data_box *l_oBox = item->m_oBoxes[m_iBoxId];
	*l_oBox = m_oOldBox;

	QList<data_box*> lst;
	lst.push_back(l_oBox);
	model->notify_size_box(m_iId, lst);
	undo_dirty();
}

void mem_class::init(data_box *i_oBox) {
	m_iBoxId = i_oBox->m_iId;
	m_oNewBox = m_oOldBox = *i_oBox;
}

///////////////////////////////////////////////////////////////////

mem_diagram_properties::mem_diagram_properties(sem_mediator* mod, int id) : mem_command(mod)
{
	m_iId = id;
}

void mem_diagram_properties::redo() {
	data_item *item = model->m_oItems[m_iId];
	item->m_oDiagramFont = m_oNewFont;
	model->notify_change_properties(NULL);
}

void mem_diagram_properties::undo() {
	data_item *item = model->m_oItems[m_iId];
	item->m_oDiagramFont = m_oOldFont;
	model->notify_change_properties(NULL);
}

