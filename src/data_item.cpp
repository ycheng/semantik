// Thomas Nagy 2007-2012 GPLV3

#include <QtDebug>
#include <QString>
#include <QCoreApplication>
#include <QtDebug>
#include <QTextEdit>
#include <QKeyEvent>
#include <QScrollBar>
#include <QCompleter>
#include <QTreeView>
#include <QHeaderView>
#include <QAction>
#include <QStandardItemModel>
#include "con.h"

#include "sem_mediator.h"
#include "data_item.h"

data_item::data_item(sem_mediator *i_oControl, int i_iId)
{
	m_iDataType = VIEW_TEXT;

	m_iId = i_iId;
	m_iColor = 0;
	m_oMediator = i_oControl;
	m_sSummary = QObject::trUtf8("Empty");
	m_sText = "";
	m_sComment = "";
	m_sDiag = "";

	m_sPicLocation = "";
	m_sPicCaption = "";
	m_sPicComment = "";

	m_iDown = NO_ITEM;

	m_iNumRows = 3;
	m_iNumCols = 3;
	m_iPicId = NO_ITEM;

	m_iYY = m_iXX = -200000.0;
}

QPixmap data_item::getPix()
{
	return m_oMediator->getPix(m_iPicId);
}

QPixmap data_item::getThumb()
{
	return m_oMediator->getThumb(m_iPicId);
}


color_scheme& data_item::get_color_scheme()
{
	if (m_oMediator->parent_of(m_iId) <= 0)
	{
		return m_oMediator->m_oColorSchemes[0];
	}
	if (m_iColor < 0)
	{
		return m_oCustom;
	}
	if (m_iColor == m_oMediator->m_oColorSchemes.size())
	{
		return m_oCustom;
	}
	if (m_iColor > m_oMediator->m_oColorSchemes.size())
	{
		qDebug()<<"invalid index color scheme "<<m_iColor;
		return m_oCustom;
	}
	if (m_iColor == 0)
	{
		return m_oMediator->m_oColorSchemes[1];
	}
	return m_oMediator->m_oColorSchemes[m_iColor];
}

color_scheme& data_item::get_color_scheme_raw()
{
	if (m_iColor < 0)
	{
		return m_oCustom;
	}
	if (m_iColor == m_oMediator->m_oColorSchemes.size())
	{
		return m_oCustom;
	}
	if (m_iColor > m_oMediator->m_oColorSchemes.size())
	{
		qDebug()<<"invalid index color scheme "<<m_iColor;
		return m_oCustom;
	}
	return m_oMediator->m_oColorSchemes[m_iColor];
}


diagram_item::diagram_item()
{
	pen_style = Qt::SolidLine;
	border_width = 1;
}

data_box::data_box(int id) : diagram_item()
{
	m_iId = id;
	m_bIsEnd = false;
	m_bIsVertical = false;
	m_iType = data_box::ACTIVITY;
	m_iWW = 80;
	m_iHH = 30;
	color = QColor("#a7e89b");
}

void diagram_item::setBorderWidth(int bw)
{
	border_width = bw;
}

void diagram_item::setPenStyle(Qt::PenStyle st)
{
	pen_style = st;
}

void diagram_item::setColor(QColor co)
{
	color = co;
}

data_link::data_link() : diagram_item()
{
	color = QColor(Qt::black);
	m_iParentPos = NORTH;
	m_iChildPos = NORTH;
}

void data_link::copy_from(const data_link& i_oLink)
{
	m_sLink       = i_oLink.m_sLink;
	m_sCaption    = i_oLink.m_sCaption;
	m_iParent     = i_oLink.m_iParent;
	m_iChild      = i_oLink.m_iChild;
	m_iParentPos  = i_oLink.m_iParentPos;
	m_iChildPos   = i_oLink.m_iChildPos;
	m_oStartPoint = i_oLink.m_oStartPoint;
	m_oEndPoint   = i_oLink.m_oEndPoint;
	m_oOffsets    = i_oLink.m_oOffsets;

	border_width  = i_oLink.border_width;
	pen_style     = i_oLink.pen_style;
	color         = i_oLink.color;
}

bool data_link::equals(const data_link& i_oLink)
{
	if (m_iParent != NO_ITEM)
	{
		if (m_iParent != i_oLink.m_iParent || m_iParentPos != i_oLink.m_iParentPos)
			return false;
	}
	else
	{
		if (m_oStartPoint != i_oLink.m_oStartPoint)
			return false;
	}

	if (m_iChild != NO_ITEM)
	{
		if (m_iChild != i_oLink.m_iChild || m_iChildPos != i_oLink.m_iChildPos)
			return false;
	}
	else
	{
		if (m_oEndPoint != i_oLink.m_oEndPoint)
			return false;
	}

	if (m_oOffsets.size() != i_oLink.m_oOffsets.size())
		return false;

	for (int i = 0; i < m_oOffsets.size(); ++i)
	{
		if (m_oOffsets.at(i) != i_oLink.m_oOffsets.at(i))
			return false;
	}

	return true;
}

/*
#include "main.moc"
*/
