// Thomas Nagy 2007-2011 GPLV3

#include <QtDebug>
#include <QAction>
#include <QMenu>
#include <QMouseEvent>
#include <QSpinBox>
#include<QCoreApplication>

#include "con.h"
#include "table_dialog.h"
#include "data_item.h"
#include "sem_model.h"
#include "table_view.h"

table_view::table_view(QWidget *i_oParent, sem_model *i_oControl) : QTableWidget(0, 0, i_oParent)
{
	m_oControl = i_oControl;
	m_bFreeze = false;

	m_oAddRowAct = new QAction(trUtf8("Add Row"), this);
	m_oAddRowAct->setStatusTip(trUtf8("Add a row"));
        connect(m_oAddRowAct, SIGNAL(triggered()), this, SLOT(add_row()));
	insertAction(0, m_oAddRowAct);

	m_oAddColAct = new QAction(trUtf8("Add Column"), this);
	m_oAddColAct->setStatusTip(trUtf8("Add a column"));
        connect(m_oAddColAct, SIGNAL(triggered()), this, SLOT(add_column()));
	insertAction(0, m_oAddColAct);


	m_oRmRowAct = new QAction(trUtf8("Remove Row"), this);
	m_oRmRowAct->setStatusTip(trUtf8("Remove a row"));
        connect(m_oRmRowAct, SIGNAL(triggered()), this, SLOT(rm_row()));
	insertAction(0, m_oRmRowAct);

	m_oRmColAct = new QAction(trUtf8("Remove Column"), this);
	m_oRmColAct->setStatusTip(trUtf8("Remove a column"));
        connect(m_oRmColAct, SIGNAL(triggered()), this, SLOT(rm_column()));
	insertAction(0, m_oRmColAct);

	QAction *m_oClearAct = new QAction(trUtf8("Redimension"), this);
	m_oClearAct->setStatusTip(trUtf8("Change table dimensions"));
        connect(m_oClearAct, SIGNAL(triggered()), this, SLOT(resize_table()));
	insertAction(0, m_oClearAct);


        QPalette l_oPalette = palette();
        l_oPalette.setBrush(QPalette::Inactive, QPalette::Base, QColor(230, 230, 230));
        l_oPalette.setBrush(QPalette::Disabled, QPalette::Base, QColor(230, 230, 230));
	setPalette(l_oPalette);

	connect(this, SIGNAL(cellChanged(int, int)), this, SLOT(cell_changed(int, int)));
}

/*
void table_view::synchro_doc(const hash_params& i_o)
{
	int l_iCmd = i_o[data_commande].toInt();
	switch (l_iCmd)
	{
		case cmd_select_item:
			{
				m_iId = i_o[data_id].toInt();
				setEnabled(m_iId > NO_ITEM);

			}
			break;
		default:
			break;
	}
}
*/

void table_view::mousePressEvent(QMouseEvent *i_oEv)
{
	if (i_oEv->button() == Qt::RightButton)
	{
		bool l_b = currentRow()>=0 && currentColumn()>=0;
		m_oRmColAct->setEnabled(l_b);
		m_oRmRowAct->setEnabled(l_b);

		QMenu::exec(actions(), i_oEv->globalPos());
	}
	else
	{
		QTableWidget::mousePressEvent(i_oEv);
	}
}

void table_view::cell_changed(int i_iRow, int i_iCol)
{
	if (m_bFreeze) return;
	Q_ASSERT(m_iId != NO_ITEM);

	QString l_sText;
	QTableWidgetItem *l_oItem = item(i_iRow, i_iCol);
	if (l_oItem) l_sText = l_oItem->text();
	else l_sText = "";

	data_item *l_oData = m_oControl->m_oItems.value(m_iId);
	QList<data_table_item> changed;

	bool found = false;
	foreach (data_table_item l_o, l_oData->m_oTableData) {
		if (l_o.m_iRow == i_iRow && l_o.m_iCol == i_iCol)
		{
			found = true;
			data_table_item t;
			t.m_iRow = i_iRow;
			t.m_iCol = i_iCol;
			t.m_sText = l_sText;
			changed.push_back(t);
		}
		else
		{
			changed.push_back(l_o);
		}
	}
	if (!found)
	{
		data_table_item t;
		t.m_iRow = i_iRow;
		t.m_iCol = i_iCol;
		t.m_sText = l_sText;
		changed.push_back(t);
	}

	mem_table *tmp = new mem_table(m_oControl);
	tmp->m_iId = m_iId;
	tmp->oldNRows = tmp->newNRows = l_oData->m_iNumRows;
	tmp->oldNCols = tmp->newNCols = l_oData->m_iNumCols;
	tmp->oldData = l_oData->m_oTableData;
	tmp->newData = changed;
	tmp->apply();
}

void table_view::add_row()
{
	insertRow(0);
	data_item *l_oData = m_oControl->m_oItems.value(m_iId);
	l_oData->m_iNumRows = rowCount();
}

void table_view::add_column()
{
	insertColumn(0);
	data_item *l_oData = m_oControl->m_oItems.value(m_iId);
	l_oData->m_iNumCols = columnCount();
}

void table_view::rm_row()
{
	removeRow(0);
	data_item *l_oData = m_oControl->m_oItems.value(m_iId);
	l_oData->m_iNumRows = rowCount();
}

void table_view::rm_column()
{
	removeColumn(0);
	data_item *l_oData = m_oControl->m_oItems.value(m_iId);
	l_oData->m_iNumCols = columnCount();
}

void table_view::resize_table()
{
	table_dialog l_oGen(this);
	l_oGen.m_oRows->setValue(rowCount());
	l_oGen.m_oCols->setValue(columnCount());
	if (l_oGen.exec() == QDialog::Accepted)
	{
		data_item *l_oData = m_oControl->m_oItems.value(m_iId);
		mem_table *tmp = new mem_table(m_oControl);
		tmp->m_iId = m_iId;
		tmp->oldNRows = l_oData->m_iNumRows;
		tmp->oldNCols = l_oData->m_iNumCols;
		tmp->newNRows = l_oGen.m_oRows->value();
		tmp->newNCols = l_oGen.m_oCols->value();
		tmp->oldData = tmp->newData = l_oData->m_oTableData;
		tmp->apply();
	}
}

void table_view::notify_table(int id)
{
	if (m_bFreeze)
	{
		return;
	}
	m_bFreeze = true;

	data_item *l_oData = m_oControl->m_oItems.value(id);

	if (l_oData->m_iNumRows != rowCount() || l_oData->m_iNumCols != columnCount())
	{
		setRowCount(l_oData->m_iNumRows);
		setColumnCount(l_oData->m_iNumCols);
	}

	foreach (data_table_item t, l_oData->m_oTableData)
	{
		QTableWidgetItem *it = item(t.m_iRow, t.m_iCol);
		if (!it)
		{
			it = new QTableWidgetItem();
			setItem(t.m_iRow, t.m_iCol, it);
		}
		it->setText(t.m_sText);
	}
	repaint();
	m_bFreeze = false;
}

void table_view::notify_select(const QList<int>& unsel, const QList<int>& sel)
{
	m_bFreeze = true;

	bool one = (sel.size() == 1);
	if (one) {
		m_iId = sel.at(0);
		data_item *l_oData = m_oControl->m_oItems.value(m_iId);

		Q_ASSERT(l_oData!=NULL);

		setRowCount(l_oData->m_iNumRows);
		setColumnCount(l_oData->m_iNumCols);

		foreach(data_table_item l_o, l_oData->m_oTableData)
		{
			QTableWidgetItem *l_oItem = new QTableWidgetItem();
			l_oItem->setText(l_o.m_sText);
			setItem(l_o.m_iRow, l_o.m_iCol, l_oItem);
		}
	} else {
		m_iId = NO_ITEM;
		setRowCount(0);
		setColumnCount(0);
	}
	repaint();
	m_bFreeze = false;
}

#include "table_view.moc"

