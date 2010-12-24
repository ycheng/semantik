/// Thomas Nagy 2007-2009 GPLV3

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOption>
#include <QColor>
#include <QPen>
#include <QtDebug>

#include <math.h>

#include "sem_model.h" 
#include  "con.h"
#include "canvas_item.h"
#include  "canvas_link.h"
#include "canvas_view.h"

canvas_link::canvas_link(canvas_view *i_oGraphWidget, canvas_item *i_oFrom, canvas_item *i_oTo)
	: QGraphicsPathItem(), m_oGraph(i_oGraphWidget)
{
	setFlags(ItemIsMovable & ItemIsSelectable);

	m_oFrom = i_oFrom;
	m_oTo = i_oTo;

	m_bSel = false;

	m_oFrom->add_link(this);
	m_oTo->add_link(this);

	setZValue(50);
	//setRect(0, 0, 20, 20);
	//m_oColor = QColor("#FFFCD5");
	//m_oColor = QColor("#ABFBC7");

	m_oColor = QColor("#000000");
	setBrush(m_oColor);

	/*
	QPen l_oPen;
	l_oPen.setWidth(5);
	setPen(l_oPen);
	*/
	i_oGraphWidget->scene()->addItem(this);
	//setParentItem(m_oFrom);
}

void canvas_link::rm_link()
{
	m_oFrom->rm_link(this);
	m_oTo->rm_link(this);
}

void canvas_link::update_pos()
{
	if (m_oFrom->m_oGraph->m_oControl->m_iConnType == 0)
	{
		update_triangle();
	}
	else
	{
		update_spline();
	}
}

void canvas_link::update_selected()
{
	bool l_b = (m_oFrom->m_bSel || m_oTo->m_bSel);
	if (m_bSel != l_b)
	{
		m_bSel = l_b;
		if (m_bSel) setBrush(QBrush(Qt::yellow)); else setBrush(Qt::black);
	}
}

void canvas_link::update_triangle()
{
	QRectF l_oR1 = m_oFrom->boundingRect();
        QRectF l_oR2 = m_oTo->boundingRect();

        double _x1 = m_oFrom->x() + l_oR1.width()/2.;
        double _y1 = m_oFrom->y() + l_oR1.height()/2.;

	double _x2 = m_oTo->x() + l_oR2.width()/2.;
	double _y2 = m_oTo->y() + l_oR2.height()/2.;

        double _X1 = _x2 - _x1;
        double _Y1 = _y2 - _y1;

        double l_oDist = 1;
	double _coeff = 0;
	if (_Y1*_Y1>_X1*_X1) _coeff = _Y1*_Y1/(_Y1*_Y1+_X1*_X1);
	else _coeff = 1 - _X1*_X1/(_Y1*_Y1+_X1*_X1);

	if (isnan(_coeff))
	{
		QPolygonF _polygon;
		_polygon
		<<QPointF(1, 1)
		<<QPointF(1, 1)
		<<QPointF(1, 1)
		<<QPointF(1, 1);

		QPainterPath l_oPath;
		l_oPath.addPolygon(_polygon);
		setPath(l_oPath);
		return;
	}

        double _deltax = l_oDist * sqrt( _coeff );
        double _deltay = l_oDist * sqrt( 1 - _coeff );

        if (_X1 != 0) if (_Y1/_X1 < 0) _deltay *= -1;

        double _s = m_oFrom->m_oGraph->m_oControl->m_dTriSize;

	QPolygonF _polygon;
	_polygon
	<<QPointF(_x1 + _s*_deltax, _y1 - _s*_deltay)
	<<QPointF(_x2 + _deltax, _y2 - _deltay)
	<<QPointF(_x2 - _deltax, _y2 + _deltay)
	<<QPointF(_x1 - _s*_deltax, _y1 + _s*_deltay);

	//qDebug()<<_polygon;

	QPainterPath l_oPath;
	l_oPath.addPolygon(_polygon);
	setPath(l_oPath);
}

#define _C_ 4.5

void canvas_link::update_spline()
{
	QRectF l_oR1 = m_oFrom->boundingRect();
        QRectF l_oR2 = m_oTo->boundingRect();

        double _x1 = m_oFrom->x() + l_oR1.width()/2.;
        double _y1 = m_oFrom->y() + l_oR1.height()/2.;

	double _x2 = m_oTo->x() + l_oR2.width()/2.;
	double _y2 = m_oTo->y() + l_oR2.height()/2.;

        double _X1 = _x2 - _x1;
        double _Y1 = _y2 - _y1;

        double l_oDist = 1;
	double _coeff = 0;
	if (_Y1*_Y1>_X1*_X1) _coeff = _Y1*_Y1/(_Y1*_Y1+_X1*_X1);
	else _coeff = 1 - _X1*_X1/(_Y1*_Y1+_X1*_X1);

	if (isnan(_coeff))
	{
		QPolygonF _polygon;
		_polygon
		<<QPointF(1, 1)
		<<QPointF(1, 1)
		<<QPointF(1, 1)
		<<QPointF(1, 1);

		QPainterPath l_oPath;
		l_oPath.addPolygon(_polygon);
		setPath(l_oPath);
		return;
	}

        double _deltax = l_oDist * sqrt( _coeff );
        double _deltay = l_oDist * sqrt( 1 - _coeff );

        if (_X1 != 0) if (_Y1/_X1 < 0) _deltay *= -1;


	QPainterPath l_oPath;
	l_oPath.moveTo(_x1 + _C_ * _deltax, _y1 - _C_ * _deltay);

	if (_x2 < _x1)
	{
		l_oPath.quadTo(_x2 + l_oR2.width(), _y2 + l_oR2.height()/2. -0.5,       _x2 + l_oR2.width()/2. - 3, _y2 + l_oR2.height()/2.);
		l_oPath.quadTo(_x2 + l_oR2.width(), _y2 + l_oR2.height()/2. -0.5,       _x1 - _C_ * _deltax, _y1 + _C_ * _deltay);
	}
	else
	{
		l_oPath.quadTo(_x2 - l_oR2.width(), _y2 + l_oR2.height()/2. -0.5,       _x2 - l_oR2.width()/2. + 3, _y2 + l_oR2.height()/2.);
		l_oPath.quadTo(_x2 - l_oR2.width(), _y2 + l_oR2.height()/2. -0.5,       _x1 - _C_ * _deltax, _y1 + _C_ * _deltay);
	}

	setPath(l_oPath);
}

