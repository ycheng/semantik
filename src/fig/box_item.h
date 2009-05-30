// Thomas Nagy 2007-2009 GPLV3


#ifndef BOX_ITEM_H
#define BOX_ITEM_H

#include <QGraphicsRectItem>
#include "con.h"

class QTextDocument;
class box_view;
class box_item : public QGraphicsRectItem
{
	public:
		box_item(box_view*);
		~box_item();

		int type() const { return gratype(37); }

		void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *);
                QTextDocument *m_oDoc;
                QTextCursor *m_oCursor;
		bool m_bEdit;

		qreal m_iXX;
		qreal m_iYY;

		void move_by(qreal, qreal);
		void set_pos(QPointF i_oP);
		void set_pos(qreal i_iXX, qreal i_iYY);

		int m_iId;

		box_view *m_oControl;

		void focus_out(QFocusEvent*);
		void focus_in();

		void keyPressEvent(QKeyEvent* i_oEv);
		bool moveKey(QKeyEvent* i_oEv);
		void keyReleaseEvent(QKeyEvent *i_oEv);
		void inputMethodEvent(QInputMethodEvent *);
};

#endif // BOX_ITEM_H

