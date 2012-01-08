// Thomas Nagy 2007-2012 GPLV3

#ifndef DATA_ITEM_H
#define DATA_ITEM_H

#include <QPair>
#include <QPixmap>
#include <QList>
#include <QPoint>
#include <QMap>
#include <QSyntaxHighlighter>
#include <QTextDocument>

#include "color_scheme.h"
#include "con.h"

class sem_mediator;

class diagram_item
{
	public:
	diagram_item();

	void setBorderWidth(int);
	void setPenStyle(Qt::PenStyle);
	void setColor(QColor);

	int border_width;
	Qt::PenStyle pen_style;
	QColor color;
};

class data_link : public diagram_item
{
	public:
	data_link();
	QString m_sLink;
	QString m_sCaption;
	int m_iParent;
	int m_iChild;
	int m_iParentPos;
	int m_iChildPos;
	QList<QPoint> m_oOffsets;
};

class data_box : public diagram_item
{
	public:
	data_box(int id);
	int m_iId;
	QString m_sText;
	double m_iXX;
	double m_iYY;
	double m_iWW;
	double m_iHH;
};

class data_item
{
	public:
		data_item(sem_mediator *i_oControl, int i_iId);
		sem_mediator* m_oMediator;

		QString m_sSummary;
		QString m_sText;
		// length of the plain text code
		int m_iTextLength;
		QString m_sComment;

		QString m_sPicLocation;
		QString m_sPicCaption;
		QString m_sPicComment;


		int m_oPixId;
		QPixmap m_oPix;
		QPixmap m_oThumbnail;

		int m_iId;
		bool m_bSelected;
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
		QHash<QPair<int, int>, QString> m_oTableData;
		QString m_sHints;

		bool load_from_path(const QString &);

		QHash<int, data_box*> m_oBoxes;
		QList<data_link*> m_oLinks;
};

#endif

