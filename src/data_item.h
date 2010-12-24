// Thomas Nagy 2007-2009 GPLV3

#ifndef DATA_ITEM_H
#define DATA_ITEM_H

#include <QPixmap>
#include <QList>
#include <QMap>
#include <QSyntaxHighlighter>
#include <QTextDocument>

#include "color_scheme.h"
#include "con.h"

class sem_model;

class data_table_item //:
{
	public:
	int m_iRow;
	int m_iCol;
	QString m_sText;
};

class data_link //:
{
	public:
	QString m_sLink;
	QString m_sCaption;
};

class data_item //:
{
	public:
		data_item(sem_model *i_oControl, int i_iId);
		sem_model* m_oControl;

		QString m_sSummary;
		QString m_sText;
		// length of the plain text code
		int m_iTextLength;
		QString m_sComment;

		QString m_sPicLocation;
		QString m_sPicCaption;
		QString m_sPicComment;

		QPixmap m_oPix;
		QPixmap m_oThumbnail;

		int m_iId;
		double m_iXX;
		double m_iYY;
		double m_iWW;
		double m_iHH;

		int m_iDataType;

		QList<QString> m_oFlags;

		int m_iColor;
		int m_iDown; // item to use when going down (keyboard navigation)
		color_scheme m_oCustom;

		color_scheme& get_color_scheme();
		color_scheme& get_color_scheme_raw();


		QString m_sDiag;

		int m_iNumRows;
		int m_iNumCols;
		QList<data_table_item> m_oTableData;
		QString m_sHints;

		bool load_from_path(const QString &);
};

#endif

