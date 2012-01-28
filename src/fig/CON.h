#ifndef CON_h
#define CON_h

#include <QGraphicsItem>
#include "data_item.h"

#define JUST 2

class connectable
{
	public:
		connectable() { m_oBox = NULL; };
		virtual ~connectable() {};
		int m_iId;
		virtual QRectF rect() const { return QRectF(); }
		virtual void update_data() {};
		virtual int choose_position(const QPointF&p, int id=-1) { return 0; };
		virtual QPoint get_point(int position) { return QPoint(0, 0); };
		data_box *m_oBox;
};

#endif

