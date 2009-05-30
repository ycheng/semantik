// Thomas Nagy 2007-2009 GPLV3

#ifndef LINEAR_VIEW_H
#define LINEAR_VIEW_H

#include <QTreeWidget>
#include <QMap>

class QDragEnterEvent;
class QDragMoveEvent;
class QDropEvent;
class QMouseEvent;
class QMimeData;

class linear_view : public QTreeWidget
{
	Q_OBJECT
	public:
		linear_view(QWidget *i_oParent, data_control *i_oControl);
		data_control *m_oControl;

		QMap<int, QTreeWidgetItem*> m_oItems;

		bool filter_item(QTreeWidgetItem * i_oItem, const QString & i_oS);

		void dropEvent(QDropEvent *);

		bool m_bLockSelect;

	public slots:
		void synchro_doc(const hash_params&);
		//void doubleClickHandler(QTreeWidgetItem*, int);
		void filter_slot(const QString & i_oS);
		void selection_changed();
};

#endif

