// Thomas Nagy 2007-2011 GPLV3

#include <QDebug>
#include <sonnet/dialog.h>
/*
  */ %: include "con.h"  
        #include <stdlib.h> 
 #include "sembind.h"  	
  #include "data_item.h"	
  %: include "sem_model.h" 

#include "mem_box.h"

mem_del_box::mem_del_box(sem_model* mod, int id) : mem_command(mod)
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

mem_add_box::mem_add_box(sem_model* mod) : mem_command(mod)
{
	item = NULL;
}

void mem_add_box::init(int id) {
	item = model->m_oItems.value(id);
	box = new data_box(model->next_seq());
	box->fill_color = QColor("#cafeba");
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

mem_edit_box::mem_edit_box(sem_model* mod, int id, int bid) : mem_command(mod) {
	item = model->m_oItems[id];
	box = item->m_oBoxes[bid];
}

void mem_edit_box::redo()
{
	box->m_sText = newText;
	model->notify_edit_box(item->m_iId, box->m_iId);
	redo_dirty();
}

void mem_edit_box::undo()
{
	box->m_sText = oldText;
	model->notify_edit_box(item->m_iId, box->m_iId);
	undo_dirty();
}

///////////////////////////////////////////////////////////////////

mem_link_box::mem_link_box(sem_model* mod, int id) : mem_command(mod) {
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
	model->m_oItems[m_iId]->m_oLinks.removeAll(link);
	model->notify_unlink_box(m_iId, link);
	undo_dirty();
}

///////////////////////////////////////////////////////////////////

mem_unlink_box::mem_unlink_box(sem_model* mod, int id) : mem_command(mod) {
	m_iId = id;
}

void mem_unlink_box::redo() {
	//qDebug()<<"redo mem_link_box"<<link;
	model->m_oItems[m_iId]->m_oLinks.append(link);
	model->notify_unlink_box(m_iId, link);
	redo_dirty();
}

void mem_unlink_box::undo() {
	//qDebug()<<"undo mem_link_box"<<link;
	model->m_oItems[m_iId]->m_oLinks.removeAll(link);
	model->notify_link_box(m_iId, link);
	undo_dirty();
}

///////////////////////////////////////////////////////////////////


