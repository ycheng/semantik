// Thomas Nagy 2007-2012 GPLV3


#ifndef BOX_VIEW_H
#define BOX_VIEW_H

#include <KUrl>
#include <QGraphicsView>
#include <QList>
#include <QPoint>
#include "con.h"

class QActionGroup;
class connectable;
class box_item;
class box_link;
class data_link;
class data_box;
class diagram_item;
class sem_mediator;
class box_view : public QGraphicsView
{
	Q_OBJECT
	public:
		box_view(QWidget*, sem_mediator *);
		~box_view();

		void clear_diagram();

		QList<box_link*> m_oLinks;

		QMap<int, connectable*> m_oItems;


		QPointF m_oLastPoint;
		QPointF m_oLastMovePoint;
		QPoint m_oScrollPoint;
		bool m_bPressed;

		QPointF m_oOffsetPoint;

		void check_canvas_size();

		QMenu* m_oMenu;
		//QMenu* m_oWidthMenu;
		//QMenu* m_oStyleMenu;
		QMenu* m_oAddBoxMenu;
		QMenu* m_oAlignMenu;
		QMenu* m_oSizeMenu;
		QMenu* m_oFileMenu;

		QAction *m_oAddItemAction;
		QAction *m_oAddLabel;
		QAction *m_oAddComponent;
		QAction *m_oAddActor;
		QAction *m_oAddUsecase;
		QAction *m_oAddNode;
		QAction *m_oAddDotStart;
		QAction *m_oAddDotEnd;
		QAction *m_oAddDecision;
		QAction *m_oAddParallelHorizontal;
		QAction *m_oAddParallelVertical;
		QAction *m_oFileExport;
		QAction *m_oFileImport;

		QAction *m_oPropertiesAction;
		QAction *m_oDeleteAction;
		QAction *m_oColorAction;
		QAction *m_oMoveUpAction;
		QAction *m_oMoveDownAction;

		void focusInEvent(QFocusEvent *);
		void focusOutEvent(QFocusEvent *);

		void enable_menu_actions(); // like check_actions, but only for the popup menu

		int m_iId; // the item this diagram belongs to

		void sync_view();
		void from_string(const QString &);
		void resizeEvent(QResizeEvent*);

		sem_mediator *m_oMediator;

		box_link *m_oCurrent;

		int m_bScroll;

		QActionGroup *m_oWidthGroup;
		QActionGroup *m_oStyleGroup;
		QActionGroup *m_oAlignGroup;
		QActionGroup *m_oSizeGroup;

		void keyPressEvent(QKeyEvent *i_oEvent);
		void keyReleaseEvent(QKeyEvent *i_oEvent);

		void mousePressEvent(QMouseEvent *);
		void mouseMoveEvent(QMouseEvent*);
		void mouseReleaseEvent(QMouseEvent*);
		void mouseDoubleClickEvent(QMouseEvent*);
		void wheelEvent(QWheelEvent*);

		//void edit_off();

		int next_seq();
		int num_seq;

		void message(const QString &, int);
		bool m_bDisableGradient;
		bool m_bShowFileMenu;
		void init_menu();
		KUrl m_oCurrentUrl;

	public slots:
		void change_colors(QAction* i_oAct);
		void enable_actions(); // used on focus in
		void slot_delete();
		void slot_add_item();
		void slot_color();
		void slot_edit_properties();
		void slot_align();
		void slot_size();

		void slot_move_up();
		void slot_move_down();

		void slot_penstyle();
		void slot_penwidth();

		void slot_add_element();

		void notify_focus(void* ptr);
		void notify_add_item(int);
		void notify_add_box(int, int);
		void notify_del_box(int, int);
		void notify_edit_box(int, int);
		void notify_link_box(int id, data_link *link);
		void notify_unlink_box(int id, data_link* link);
		void notify_box_props(int id, const QList<diagram_item*>&);
		void notify_pos_box(int, const QList<data_box*>&);
		void notify_size_box(int, const QList<data_box*>&);
		void notify_change_link_box(int id, data_link*link);

		void notify_select(const QList<int>& unsel, const QList<int>& sel);
		void notify_export_item(int);

		bool slot_import_from_file();
		void slot_export_to_file();
		void slot_save();

	signals:
		void sig_message(const QString &, int);
		void sig_Url(const KUrl&);
};

#endif

