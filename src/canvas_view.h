// Thomas Nagy 2007-2011 GPLV3

#ifndef CANVAS_VIEW_H
#define CANVAS_VIEW_H

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
class sem_model;
class canvas_item;
class rubber_line;
class canvas_view : public QGraphicsView
{
	Q_OBJECT
	public:
		canvas_view(QWidget *i_oWidget, sem_model* i_oControl);
		void item_moved();

		QMap<int, canvas_item*> m_oItems;

		enum mode_type
		{
			no_mode = 0,
			select_mode = 1,
			link_mode = 2,
			sort_mode = 3,
			scroll_mode = 4,
		};

		void set_mode(mode_type i_o);

		QMenu* m_oMenu;
		QMenu* m_oColorMenu;
		QMenu* m_oDataMenu;


		QAction* m_oTextType, *m_oDiagramType, *m_oTableType, *m_oImageType;


		bool m_bDeleting;
		mode_type m_iMode;

		//void keyPressEvent(QKeyEvent *i_oEvent);
		//void keyReleaseEvent(QKeyEvent *i_oEvent);
		//void focusOutEvent(QFocusEvent *i_oEv);
		//void focusInEvent(QFocusEvent *i_oEv);
		void wheelEvent(QWheelEvent *i_oEvent);
		void mouseDoubleClickEvent(QMouseEvent *i_oEv);
		void mousePressEvent(QMouseEvent *i_oEv);
		void mouseMoveEvent(QMouseEvent *i_oEv);
		void mouseReleaseEvent(QMouseEvent *i_oEv);


		bool m_bPressed;
		QPoint m_oLastPressPoint;
		QPointF m_oLastMovePoint;

		QWidget *m_oSemantikWindow;

		QRubberBand* m_oRubbery;
		rubber_line * m_oRubberLine;
		QList<canvas_item*> selection();

		void deselect_all(bool i_oSignal=true);
		void show_sort(int i_iId, bool i_b);
		void add_select(canvas_item*, bool i_oSignal=true);
		void rm_select(canvas_item*, bool i_oSignal=true);
		void notify_select(bool i_oSignal=true);
		void enable_menu_actions(); // like check_actions, but only for the popup menu

		double compute_height(QMap<int, double> &map, QMap<int, QList<int> >&children, int id);
		void compute_width(QMap<int, double> &map, QMap<int, QList<int> >&children, int id, int level);
		void pack(QMap<int, double> &width, QMap<int, double> &height, QMap<int, QList<int> >&children, int id, int level, int left);

		void check_selected();

		//void scaleView(qreal i_oScaleFactor);

		sem_model *m_oControl;

		QVector<canvas_item*> m_oRect;

		QPointF m_oLastPoint;

		int m_iSortCursor;

		void move_sel(int i_iX, int i_iY);

		mode_type m_iLastMode;

		void check_canvas_size();

		//bool event(QEvent *i_oEvent);

		void enable_all(bool);

		QAction *m_oAddItemAction;
		QAction *m_oEditAction;
		QAction *m_oDeleteAction;

	public slots:
		void synchro_doc(const hash_params&);
		void change_colors(QAction*);
		void fit_zoom();
		void change_flags(QAction*);
		void slot_edit();
		void slot_add_item();
		void enable_actions(); // used on focus in
		void slot_add_sibling();
		void slot_delete();
		void slot_move();
		void slot_sel();
		void slot_next_root();
		void zoom_in();
		void zoom_out();
		void toggle_fullscreen();


		void notify_add_item(int id);
		void notify_delete_item(int id);
		void notify_link_items(int id1, int id2);
		void notify_unlink_items(int id1, int id2);
		void notify_select(const QList<int>& unsel, const QList<int>& sel);
		void notify_move(const QList<int>&sel, const QList<QPointF>&pos);
		void sync_flags();
		void sync_colors();

		void slot_change_data();
		void reorganize();

		//void selection_changed();
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

