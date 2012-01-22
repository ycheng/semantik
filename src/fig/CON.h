#ifndef CON_h
#define CON_h

#include <QGraphicsItem>

#define JUST 2

class connectable {
	public:
		int m_iId;
		virtual QRectF rect() const { return QRectF(); }
};

#endif

