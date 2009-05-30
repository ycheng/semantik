// Thomas Nagy 2007-2009 GPLV3


#ifndef BOX_VIEW_H
#define BOX_VIEW_H

#include <QGraphicsView>
#include <QList>
#include <QPoint>
#include "con.h"

class QActionGroup;
class box_item;
class box_link;
class data_control;
class box_view : public QGraphicsView
{
	Q_OBJECT
	public:
		box_view(QWidget*, data_control *);
		~box_view();

		void mousePressEvent(QMouseEvent *);
		void mouseDoubleClickEvent(QMouseEvent*);
		void wheelEvent(QWheelEvent*);
		void clear_diagram();

		void keyPressEvent(QKeyEvent*);
		void keyReleaseEvent(QKeyEvent*);
		bool event(QEvent *i_oEvent);

		QList<QGraphicsItem*> m_oSelected;
		QList<box_item*> m_oItems;
		QList<box_link*> m_oLinks;

		void deselect_all();
		void add_select(QGraphicsItem*);
		void rm_select(QGraphicsItem*);

		void mouseMoveEvent(QMouseEvent*);
		void mouseReleaseEvent(QMouseEvent*);

		QPointF m_oLastPoint;
		QPointF m_oLastMovePoint;
		QPoint m_oScrollPoint;
		bool m_bPressed;

		QPointF m_oOffsetPoint;

		void check_canvas_size();

		QMenu* m_oMenu;
		QMenu* m_oWidthMenu;
		QMenu* m_oStyleMenu;

		QAction *m_oAddItemAction;
		QAction *m_oEditAction;
		QAction *m_oDeleteAction;
		QAction *m_oColorAction;
		QAction *m_oMoveUpAction;
		QAction *m_oMoveDownAction;

		void focusInEvent(QFocusEvent *);
		void focusOutEvent(QFocusEvent *);

		void enable_menu_actions(); // like check_actions, but only for the popup menu

		int next_id();

		int m_iId; // the item this diagram belongs to

		// private
		int m_iIdCounter;

		QString to_string();
		void from_string(const QString &);

		data_control *m_oControl;

		box_link *m_oCurrent;

		int m_bScroll;

		QActionGroup *m_oWidthGroup;
		QActionGroup *m_oStyleGroup;

	public slots:
		void enable_actions(); // used on focus in
		void slot_delete();
		void slot_add_item();
		void slot_color();
		void slot_edit();

		void slot_move_up();
		void slot_move_down();
		void synchro_doc(const hash_params& i_o);

		void slot_penstyle();
		void slot_penwidth();
};

#endif

