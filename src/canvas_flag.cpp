// Thomas Nagy 2007-2012 GPLV3


#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOption>
#include <QColor>
#include <QPen>
#include <QtDebug>

#include <QSvgRenderer>

#include <math.h>

#include "data_item.h"
#include "sem_model.h"
#include "con.h"
#include "canvas_item.h"
#include "canvas_flag.h"
#include "canvas_view.h"


canvas_flag::canvas_flag(canvas_view *i_oGraphWidget, canvas_item *i_oFrom)
	: QGraphicsRectItem(), m_oGraph(i_oGraphWidget)
{
	setFlags(ItemIsMovable & ItemIsSelectable);
	m_oItem = i_oFrom;
	setRect(0, 0, 19, 19);

	/* set the z value to 51 */
	setZValue(51);

	i_oGraphWidget->scene()->addItem(this);
	setParentItem(m_oItem);
	setCacheMode(QGraphicsItem::DeviceCoordinateCache);
}

void canvas_flag::paint(QPainter *i_oPainter, const QStyleOptionGraphicsItem *i_oOption, QWidget * i_oW)
{
	int l_iIndex = 0;
	for (int i=0; i<m_oItem->m_oFlags.size(); ++i)
	{
		if (this == m_oItem->m_oFlags[i]) { l_iIndex = i; break; }
	}
	data_item *l_oDataItem = m_oGraph->m_oMediator->m_oItems.value(m_oItem->Id());
	QString l_sName = l_oDataItem->m_oFlags[l_iIndex];

	for (int i=0; i<m_oGraph->m_oMediator->m_oFlagSchemes.size(); ++i)
	{
		if (m_oGraph->m_oMediator->m_oFlagSchemes[i]->m_sId == l_sName)
		{
			QSvgRenderer *l_o = m_oGraph->m_oMediator->m_oFlagSchemes[i]->_renderer();
			l_o->render(i_oPainter, boundingRect());
			break;
		}
	}
}

#ifdef WAF
#else
#include "canvas_view.moc"
#endif

