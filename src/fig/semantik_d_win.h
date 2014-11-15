// Thomas Nagy 2013-2014 GPLV3

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
class KTabWidget;
class filetree;

class diagram_document;

class semantik_d_win : public KXmlGuiWindow
{
	Q_OBJECT

	public:
		semantik_d_win(QWidget *i_oParent=0);

		diagram_document *m_oActiveDocument;
		KAction *m_oUndoAct;
		KAction *m_oRedoAct;

		KRecentFilesAction* m_oRecentFilesAct;
		KTabWidget *m_oTabWidget;
		filetree *m_oFileTree;

		void read_config();
		void write_config();
		bool queryClose();

		void wire_actions();
		void print_current(KUrl i_oUrl);

	public slots:
		void slot_open();
		void slot_enable_undo(bool, bool);
		void slot_recent(const KUrl &);

		void slot_properties();

		void slot_generate();
		bool save_tab(QWidget*);

		void slot_tip_of_day();
		void slot_add_tab();
		void slot_tab_changed(int);
		void slot_remove_tab(QWidget*);
		void slot_update_tab_text(diagram_document*, const KUrl &);

		void fit_zoom();
		void record_open_url(const KUrl&);
		void update_title();
	signals:
		void url_opened(const KUrl&);
};

#endif

