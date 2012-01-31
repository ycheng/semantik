// Thomas Nagy 2007-2012 GPLV3

#ifndef resize_h
#define resize_h

#include "box_view.h"
#include  <box_resize_point.h>

class box_resize_point;
class resizable
{
	public:
	resizable(box_view* m_oView) {
	}
	box_resize_point *m_oTopLeft;
	box_resize_point *m_oBottomRight;
	box_resize_point *m_oTopRight;
	box_resize_point *m_oBottomLeft;
};

/*
resizable::resizable(box_view* m_oView)
	{
		m_oTopLeft = new box_resize_point(m_oView);
		m_oBottomRight = new box_resize_point(m_oView);
		m_oTopRight = new box_resize_point(m_oView);
		m_oBottomLeft = new box_resize_point(m_oView);
	}
*/
#endif // resize_h

