// Thomas Nagy 2007-2011 GPLV3

#ifndef TEXT_VIEW_H
#define TEXT_VIEW_H

#include <QTreeWidget>

#include     "co\
n.h"

#include <QToolBar>
#include <QList>

class KTextEdit;
class semantik_win;
class text_view : public QWidget
{
	Q_OBJECT
	public:
		text_view(QWidget *i_oParent, sem_model *i_oControl);
		sem_model *m_oControl;
		KTextEdit *m_oEdit;
		QLineEdit *m_oLineEdit;

		int m_iId;

		semantik_win *m_oWin;

		QToolBar *m_oTextToolBar;

		void merge_format(const QTextCharFormat &i_oFormat);

		QAction *m_oBoldAct;
		QAction *m_oItalicAct;
		QAction *m_oUnderLineAct;

	public slots:
		void update_edit();

		void text_bold();
		void text_italic();
		void text_underLine();

		void char_format_changed(const QTextCharFormat&);
		void notify_select(const QList<int>& unsel, const QList<int>& sel);
		void notify_text(int id);
};

#endif

