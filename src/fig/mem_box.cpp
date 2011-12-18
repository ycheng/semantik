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

mem_del_box::mem_del_box(sem_model* mod) : mem_command(mod)
{

}

void mem_del_box::init(int id, QList<int> ids)
{

}


void mem_del_box::undo()
{

}

void mem_del_box::redo()
{

}

mem_add_box::mem_add_box(sem_model* mod) : mem_command(mod)
{
	item = NULL;
}

void mem_add_box::init(int id) {
	item = model->m_oItems[id];
	box = new data_box(model->next_seq());
}

void mem_add_box::redo()
{
	qDebug()<<"mem box add redo";
	item->m_oBoxes[box->m_iId] = box;
	model->notify_add_box(item->m_iId, box->m_iId);
	redo_dirty();
}

void mem_add_box::undo()
{
	qDebug()<<"mem box add undo";
	model->notify_del_box(item->m_iId, box->m_iId);
	item->m_oBoxes.remove(box->m_iId);
	undo_dirty();
}


