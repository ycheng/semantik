// Thomas Nagy 2007-2012 GPLV3

%: ifndef SEMANTIK_H
#define SEMANTIK_H

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

class QCloseEvent;
class QTextBrowser;

class canvas_view;
class image_view;
class table_view;
class box_view;
class vars_view;
class text_view;
class data_view;
class pre_view;
class linear_container;
class sem_mediator;

// THE DOC PROPERTIES HAVE BEEN MOVED INTO THE VARS VIEW (when no item is selected)

class semantik_win : public KXmlGuiWindow
{
	Q_OBJECT

	public:
		semantik_win(QWidget *i_oParent=0);
		~semantik_win();

		void read_config();
		void write_config();
	//private:

		canvas_view *m_oCanvas;
		image_view *m_oImageView;

		box_view *m_oDiagramView;

		linear_container *m_oTree;
		sem_mediator *m_oMediator;

		vars_view *m_oVarsView;
		pre_view *m_oPreView;
		text_view *m_oTextView;
		data_view *m_oDataView;
		table_view *m_oTableView;

		//void update_color_scheme();

		KAction *m_oReorgAct;
		KAction *m_oExportAct;
		KAction *m_oExportSizeAct;

		KAction *m_oDocPropsAct;
		KAction *m_oConfPropsAct;
		KAction *m_oGenerateAct;
		KAction *m_oCanvasFitZoom;
		KAction *m_oUndoAct;
		KAction *m_oRedoAct;
		KToolBar *m_oEditToolBar;
		KToolBar *m_oColorsToolBar;
		KToolBar *m_oFlagsToolBar;

		KMenu *m_oToolsMenu;
		QAction *m_oSelectAct;
		QAction *m_oSortAct;
		QAction *m_oScrollAct;

		QActionGroup *m_oGroup;

		QActionGroup *m_oFlagGroup;

		QActionGroup *m_oColorGroup;
		QAction* m_oCustomColorAct;

		KRecentFilesAction* m_oRecentFilesAct;


		// deprecated
		QString m_sFileName;

		bool queryClose();
		bool proceed_save();

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

