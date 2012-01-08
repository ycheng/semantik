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

	m_iYY = m_iXX = -200000.0;
}

bool data_item::load_from_path(const QString & i_sPath)
{
	QPixmap l_oPix;
	l_oPix.load(i_sPath);
	if (l_oPix.isNull()) return false;

	//m_oPix = l_oPix.scaledToHeight(300);
	m_oPix = l_oPix;
	m_oThumbnail = l_oPix.scaledToHeight(32);

	return true;
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
	color = QColor("#cafeba");
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
}


/*
#include "main.moc"
*/
