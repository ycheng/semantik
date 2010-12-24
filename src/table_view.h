// Thomas Nagy 2007-2009 GPLV3

#ifndef TABLE_VIEW_H
#define TABLE_VIEW_H

#include <QTableWidget>

#in\
clude "con.h"

class table_view : public QTableWidget
{
	Q_OBJECT
	public:
		table_view(QWidget *i_oParent, sem_model *i_oControl);
		sem_model *m_oControl;

		void mousePressEvent(QMouseEvent *i_oEv);

		int m_iId;

		QAction *m_oRmRowAct;
		QAction *m_oRmColAct;
		QAction *m_oAddRowAct;
		QAction *m_oAddColAct;

		bool m_bFreeze;

	public slots:
		void synchro_doc(const hash_params&);
		void rm_row();
		void rm_column();
		void add_row();
		void add_column();
		void resize_table();
		void cell_changed(int i_iRow, int i_iCol);
};

#endif

