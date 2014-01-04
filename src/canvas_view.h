// Thomas Nagy 2007-2012 GPLV3

#ifndef CANVAS_VIEW_H
#define CANVAS_VIEW_H

#include "KUrl"
#include <QtGui/QGraphicsView>
#include  <QGraphicsRectItem>
#include   <QVector>
#include    <QMap>
#include     <QList>
#include      "con.h"
#include       <QAction> 
#include        <QRubberBand>

class QMenu;
class canvas_link;
class sem_mediator;
class canvas_item;
class rubber_line;
class canvas_view : public QGraphicsView
{
	Q_OBJECT
	public:
		canvas_view(QWidget *i_oWidget, sem_mediator* i_oControl);
		void item_moved();

		QMap<int, canvas_item*> m_oItems;

		QMenu* m_oMenu;
		QMenu* m_oColorMenu;
		QMenu* m_oDataMenu;


		QAction* m_oTextType, *m_oDiagramType, *m_oTableType, *m_oImageType;


		bool m_bDeleting;
		bool m_bDisableGradient;

		void focusInEvent(QFocusEvent *);
		void focusOutEvent(QFocusEvent *);

		void wheelEvent(QWheelEvent *i_oEvent);
		void mouseDoubleClickEvent(QMouseEvent *i_oEv);
		void mousePressEvent(QMouseEvent *i_oEv);
		void mouseMoveEvent(QMouseEvent *i_oEv);
		void mouseReleaseEvent(QMouseEvent *i_oEv);


		bool m_bPressed;
		bool m_bScroll;

		QPoint m_oLastPressPoint;
		QPointF m_oLastMovePoint;

		QWidget *m_oSemantikWindow;

		QRubberBand* m_oRubbery;
		rubber_line * m_oRubberLine;
		QList<canvas_item*> selection();

		void deselect_all(bool i_oSignal=true);
		void show_sort(int i_iId, bool i_b);
		void enable_menu_actions(); // like check_actions, but only for the popup menu
		int batch_print_map(const KUrl& url, QPair<int, int> & size);

		double compute_height(QMap<int, double> &map, QMap<int, QList<int> >&children, int id);
		void compute_width(QMap<int, double> &map, QMap<int, QList<int> >&children, int id, int level);
		void pack(QMap<int, double> &width, QMap<int, double> &height, QMap<int, QList<int> >&children, int id, int level, int left);

		void check_selected();
		void edit_off(); // if the editing was on, turn it off

		//void scaleView(qreal i_oScaleFactor);

		sem_mediator *m_oMediator;

		QVector<canvas_item*> m_oRect;

		QPointF m_oLastPoint;

		int m_iSortCursor;
		int m_iSortId;

		void move_sel(int i_iX, int i_iY);

		void check_canvas_size();

		void enable_all(bool);

		QAction *m_oAddItemAction;
		QAction *m_oEditAction;
		QAction *m_oCancelEditAction;
		QAction *m_oDeleteAction;

		QAction *m_oMoveUpAction;
		QAction *m_oMoveDownAction;
		QAction *m_oMoveLeftAction;
		QAction *m_oMoveRightAction;
		QAction *m_oSelectUpAction;
		QAction *m_oSelectDownAction;
		QAction *m_oSelectLeftAction;
		QAction *m_oSelectRightAction;
		QAction *m_oSelectSubtreeAction;

		QAction *m_oInsertSiblingAction;
		QAction *m_oNextRootAction;

		void resizeEvent(QResizeEvent*);

	public slots:
		void export_map_size();
		void change_colors(QAction*);
		void fit_zoom();
		void change_flags(QAction*);
		void slot_toggle_edit();
		void slot_cancel_edit();
		void slot_add_item();
		void enable_actions(); // used on focus in
		void slot_add_sibling();
		void slot_delete();
		void slot_move();
		void slot_sel();
		void slot_next_root();
		void zoom_in();
		void slot_print();
		void zoom_out();
		void toggle_fullscreen();
		void slot_select_subtree();

		void notify_focus(void* ptr);
		void notify_pic(int id);
		void notify_add_item(int id);
		void notify_delete_item(int id);
		void notify_link_items(int id1, int id2);
		void notify_unlink_items(int id1, int id2);
		void notify_select(const QList<int>& unsel, const QList<int>& sel);
		void notify_move(const QList<int>&sel, const QList<QPointF>&pos);
		void notify_repaint(int id);
		void notify_edit(int id);
		void notify_flag(int id);
		void notify_sort(int id);
		void notify_open_map();
		void notify_change_data(int id);
		void notify_export_doc();
		void sync_flags();
		void sync_colors();

		void slot_change_data();
		void reorganize();

		void check_selection();

	signals:
		void sig_message(const QString &, int);
};

class rubber_line : public QRubberBand
{
	public:
	int _direction;
	rubber_line(QRubberBand::Shape, QWidget*);
	void paintEvent(QPaintEvent *);
	void setGeometry(const QRect& i_o);
};

#endif

