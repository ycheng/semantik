// Thomas Nagy 2007-2015 GPLV3

#ifndef resize_h
#define resize_h

#include <QPointF>
#include "box_view.h"
#include  <box_resize_point.h>

#define CTRLSIZE 8

class box_resize_point;
class resizable
{
	public:
	resizable() {}
	int m_iOrientation;
	virtual QPointF validate_point(box_resize_point *p, const QPointF & orig) = 0;
	virtual void commit_size(box_resize_point *p) = 0;
	virtual void freeze(bool b) = 0;
};

#endif // resize_h

