// Thomas Nagy 2007-2012 GPLV3

#include <QtDebug>
#include <QTreeWidget>
#include <QHeaderView>
#include <QDragEnterEvent>
#include <QDropEvent>

#include "data_item.h"
#include "sem_model.h"
#include "linear_view.h"
#include "con.h" 

linear_view::linear_view(QWidget *i_oParent, sem_model *i_oControl) : QTreeWidget(i_oParent)
{
	setDragEnabled(true);
	setAcceptDrops(true);
	setDropIndicatorShown(true);
	setDragDropMode(QAbstractItemView::InternalMove);
	header()->hide();
	m_oControl = i_oControl;
	connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(selection_changed()));
	m_bLockSelect = false;
}

void linear_view::notify_add_item(int id) {
	QTreeWidgetItem *l_oItem = new QTreeWidgetItem(this);
	l_oItem->setText(0, m_oControl->m_oItems.value(id)->m_sSummary);
	l_oItem->setData(0, Qt::UserRole, id);
	//l_oItem->setFlags(Qt::ItemIsDragEnabled | Qt::ItemIsEnabled | Qt::ItemIsDropEnabled);
	addTopLevelItem(l_oItem);
	m_oItems[id] = l_oItem;
}

void linear_view::notify_delete_item(int id) {
	QTreeWidgetItem *l_oItem = m_oItems.value(id);

	QTreeWidgetItem *l_oChild = NULL;
	while ( (l_oChild = l_oItem->takeChild(0)) != NULL) {
		addTopLevelItem(l_oChild);
	}
	if (l_oItem->parent())
		l_oItem->parent()->takeChild(l_oItem->parent()->indexOfChild(l_oItem));
	else
		takeTopLevelItem(indexOfTopLevelItem(l_oItem));
	delete l_oItem;
}

void linear_view::notify_link_items(int id1, int id2) {
	QTreeWidgetItem *l_oItem1 = m_oItems.value(id2);
	QTreeWidgetItem *l_oItem2 = m_oItems.value(id1);
	QTreeWidgetItem *l_oRet = takeTopLevelItem(indexOfTopLevelItem(l_oItem1));
	Q_ASSERT(l_oRet != NULL);
	l_oItem2->insertChild(l_oItem2->childCount(), l_oItem1);
	l_oItem2->setExpanded(true);

	data_item *l_o = m_oControl->m_oItems.value(id2);
	l_oItem1->setBackgroundColor(0, l_o->get_color_scheme().m_oInnerColor);
}

void linear_view::notify_unlink_items(int id1, int id2) {
	QTreeWidgetItem *l_oItem1 = m_oItems.value(id1);
	QTreeWidgetItem *l_oItem2 = m_oItems.value(id2);
	if (l_oItem1->parent() == l_oItem2)
	{
		l_oItem2->takeChild(l_oItem2->indexOfChild(l_oItem1));
		l_oItem2->setExpanded(true);
		addTopLevelItem(l_oItem1);

		data_item *l_o = m_oControl->m_oItems.value(id1);
		l_oItem1->setBackgroundColor(0, l_o->get_color_scheme().m_oInnerColor);
	}
	else if (l_oItem2->parent() == l_oItem1)
	{
		l_oItem1->takeChild(l_oItem1->indexOfChild(l_oItem2));
		l_oItem1->setExpanded(true);
		addTopLevelItem(l_oItem2);

		data_item *l_o = m_oControl->m_oItems.value(id2);
		l_oItem2->setBackgroundColor(0, l_o->get_color_scheme().m_oInnerColor);
	}
	else
	{
		Q_ASSERT(1>1);
	}
}

void linear_view::selection_changed()
{
	if (!m_bLockSelect)
	{
		QList<QTreeWidgetItem*> l_oItems = selectedItems();
		QList<int> lst;

		foreach (QTreeWidgetItem* it, l_oItems)
		{
			lst.append(it->data(0, Qt::UserRole).toInt());
		}

		mem_sel *sel = new mem_sel(m_oControl);
		sel->sel = lst;
		sel->apply();
	}
}

void linear_view::filter_slot(const QString & i_oS)
{
	foreach (QTreeWidgetItem *l_oItem, m_oItems.values())
	{
#if 0
		if (i_oS.length() < 1)
		{
			l_oItem->setHidden(false);
		}
#endif

		if (l_oItem->parent() != NULL) continue;
		filter_item(l_oItem, i_oS);
	}
}

bool linear_view::filter_item(QTreeWidgetItem * i_oItem, const QString & i_oS)
{
	bool l_bDisp = (i_oItem->text(0).indexOf(i_oS, 0, Qt::CaseInsensitive) >= 0);
	int l_iCount = i_oItem->childCount();

	for (int i=0; i<l_iCount; ++i)
	{
		QTreeWidgetItem *l_oChild = i_oItem->child(i);
		l_bDisp = filter_item(l_oChild, i_oS) || l_bDisp;
	}

	i_oItem->setHidden(!l_bDisp);
	return l_bDisp;
}

void linear_view::dropEvent(QDropEvent *i_oEv)
{
	if (i_oEv->source() == this)
	{
		QTreeWidgetItem *l_oItem = itemAt(i_oEv->pos());
		QTreeWidgetItem *l_oChild = selectedItems().at(0);
		Q_ASSERT(l_oChild != NULL);

		if (l_oChild)
		{
			int l_iId = l_oChild->data(0, Qt::UserRole).toInt();
			if (dropIndicatorPosition() == QAbstractItemView::OnItem)
			{
				if (m_oControl->parent_of(l_iId))
				{
					mem_unlink *link = new mem_unlink(m_oControl);
					link->child = l_iId;
					link->parent = m_oControl->parent_of(l_iId);
					link->apply();
				}

				if (l_oItem != NULL)
				{
					m_oControl->link_items(l_oItem->data(0, Qt::UserRole).toInt(), l_iId);
				}
			}
			else if (l_oItem != NULL)
			{
				int j = l_oItem->data(0, Qt::UserRole).toInt();
				int k = m_oControl->parent_of(j);

				// make certain they have the same parent now
				if (m_oControl->parent_of(l_iId) != k);
				{
					if (m_oControl->parent_of(l_iId))
					{
						mem_unlink *link = new mem_unlink(m_oControl);
						link->child = l_iId;
						link->parent = m_oControl->parent_of(l_iId);
						link->apply();
					}
					m_oControl->link_items(k, l_iId);
				}

				int z = 1; // offset for re-inserting the item
				if (dropIndicatorPosition() == QAbstractItemView::AboveItem)
				{
					z = 0;
				}

				int l = 0; // the index of the item drop
				for (int i=0; i<m_oControl->m_oLinks.size(); i++)
				{
					QPoint l_oP = m_oControl->m_oLinks.at(i);
					if (l_oP.x() == k) // same parent
					{
						if (l_oP.y() == j) // item found
						{
							mem_sort *srt = new mem_sort(m_oControl);
							srt->init(k, l_iId, l+z);
							srt->apply();

							break;
						}
						l++;
					}
				}
			}
			else
			{
				if (m_oControl->parent_of(l_iId))
				{
					mem_unlink *link = new mem_unlink(m_oControl);
					link->child = l_iId;
					link->parent = m_oControl->parent_of(l_iId);
					link->apply();
				}
			}
			QList<int> lst;
			lst.append(l_iId);
			mem_sel *sel = new mem_sel(m_oControl);
			sel->sel = lst;
			sel->apply();
		}
	}
	i_oEv->accept();
	i_oEv->setDropAction(Qt::CopyAction);
}

void linear_view::notify_select(const QList<int>& unsel, const QList<int>& sel) {
	m_bLockSelect = true;

	QList<QTreeWidgetItem *> l_oLst = selectedItems();
	foreach (QTreeWidgetItem* l_oItem, l_oLst)
	{
		int id = l_oItem->data(0, Qt::UserRole).toInt();
		if (!sel.contains(id))
		{
			l_oItem->setSelected(false);
		}
	}

	foreach (int id, sel)
	{
		QTreeWidgetItem *item = m_oItems.value(id);
		item->setSelected(true);
		item->setExpanded(true);
	}

	m_bLockSelect = false;
}

void linear_view::notify_repaint(int id)
{
	QTreeWidgetItem *l_oItem = m_oItems.value(id);
	data_item *l_o = m_oControl->m_oItems.value(id);
	l_oItem->setBackgroundColor(0, l_o->get_color_scheme().m_oInnerColor);
}

void linear_view::notify_sort(int id) {
	QTreeWidgetItem *l_oItem = m_oItems.value(id);
	int l_iCnt = 0;
	for (int i=0; i<m_oControl->m_oLinks.size(); i++)
	{
		QPoint l_oP = m_oControl->m_oLinks.at(i);
		if (l_oP.x() == id)
		{
			QTreeWidgetItem *l_oItem2 = m_oItems.value(l_oP.y());
			int l_iIndex = l_oItem->indexOfChild(l_oItem2);
			if (l_iIndex != l_iCnt)
			{
				l_oItem->takeChild(l_iIndex);
				l_oItem->insertChild(l_iCnt, l_oItem2);
			}
			l_iCnt++;
		}
	}
}

void linear_view::notify_edit(int id)
{
	QTreeWidgetItem *l_oItem = m_oItems.value(id);
	l_oItem->setText(0, m_oControl->m_oItems.value(id)->m_sSummary);
}

#include "linear_view.moc"

