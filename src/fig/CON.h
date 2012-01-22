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
		data_box *m_oBox;
};

#endif

