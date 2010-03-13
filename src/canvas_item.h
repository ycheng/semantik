// Thomas Nagy 2007-2009 GPLV3



#ifndef CANVAS_ITEM_H
#define CANVAS_ITEM_H

#include "con.h" 

#include <QGraphicsItem>
#include <QList>

class QTextDocument;
class QColor;
class QString;
class QSvgRenderer;
class QGraphicsSceneMouseEvent;
class canvas_view;
class canvas_link;
class canvas_sort;
class canvas_flag;

class canvas_item: public QGraphicsRectItem
{
	public:
		canvas_item(canvas_view *i_oGraphWidget, int i_iId);
		~canvas_item();

		int type() const { return CANVAS_ITEM_T; }

		void update_flags();

		QList<canvas_link*> m_oLinks;
		void add_link(canvas_link*);
		void rm_link(canvas_link*);

		/*int get_id() const {return m_iID;}
		  QString get_label() {return m_sLabel;}

		  void set_ID(int i) {m_iID=i;}
		 */

		QVariant itemChange(GraphicsItemChange i_oChange, const QVariant &i_oValue);

		QColor get_color() {return m_oColor;}
		void set_color(QColor c) {m_oColor=c;}

		void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *);


		void set_parent(canvas_item *);
		void adjustSize();

		int Id() const {return m_iId;};

		QTextDocument *m_oDoc;
		QTextCursor *m_oCursor;

		bool m_bSel;
		bool m_bEdit;
		QString m_sNum;

		void set_selected(bool);

		bool hasFocus() const;

		void keyPressEvent(QKeyEvent* i_oEv);
		bool moveKey(QKeyEvent* i_oEv);
		void keyReleaseEvent(QKeyEvent *i_oEv);
		void focus_out(QFocusEvent *i_oEv);
		void inputMethodEvent(QInputMethodEvent *);

		void focus_in();

		static QColor s_oColorRect;
		static QColor s_oColorRoot;
		static QColor s_oColorLeaf;
		static QColor s_oColorSelect;
		static QColor s_oColorRectEd;

		void update_data();
		void update_links();
		void update_color();

		QList<canvas_flag*> m_oFlags;

		inline qreal const x2() {return x() + rect().width();}
		inline qreal const y2() {return y() + rect().height();}

		canvas_view *m_oGraph;
		canvas_item *m_oParent;
		canvas_sort *m_oSort;

		int m_iId;
		QColor m_oColor;
		QColor m_oColorBackup;
		QString m_sLabel;

		//QSvgRenderer *m_oRenderer;
};

#endif

