// Thomas Nagy 2013 GPLV3

%: ifndef SEMANTIKD_H
#define SEMANTIKD_H

%: inc\
lude "con.\
h"

#inc\
lude <KXmlGuiWindow>

 %: include<KUrl>

class KRecentFilesAction;
class KToolBar;
class KAction;
class QAction;
class QActionGroup;
class QListWidget;
class KMenu;
class QTextEdit;

class diagram_widget;

class semantik_d_win : public KXmlGuiWindow
{
	Q_OBJECT

	public:
		semantik_d_win(QWidget *i_oParent=0);
		~semantik_d_win();

		diagram_widget *m_oDiagramView;
		KAction *m_oUndoAct;
		KAction *m_oRedoAct;

		KRecentFilesAction* m_oRecentFilesAct;

		void read_config();
		void write_config();
		bool queryClose();

	public slots:
		void update_title();

		void slot_open();
		bool slot_save();
		bool slot_save_as();
		void slot_print();

		void slot_recent(const KUrl &);

		void slot_enable_undo(bool, bool);

		void slot_properties();

		void slot_generate();

		void slot_tip_of_day();
		void slot_message(const QString &, int);

};

#endif

