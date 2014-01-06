// Thomas Nagy 2007-2014 GPLV3

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
	enum Direction { NORTH=1, WEST=2, SOUTH=4, EAST=8, COORD=15 };
	data_link();
	QString m_sLink;
	QString m_sCaption;

	int m_iParent;
	int m_iChild;
	int m_iParentPos;
	int m_iChildPos;

	bool m_iLeftArrow;
	bool m_iRightArrow;
	int m_iLineType;

	QPoint m_oStartPoint;
	QPoint m_oEndPoint;
	QList<QPoint> m_oOffsets;

	void copy_from(const data_link&);
	bool equals(const data_link&);
};

class data_box : public diagram_item
{
	public:
	data_box(int id);
	int m_iId;
	QString m_sText;
	int m_iXX;
	int m_iYY;
	int m_iWW;
	int m_iHH;
	bool m_bIsEnd;
	bool m_bIsVertical;
	enum IType { ACTIVITY=1, ACTIVITY_START=2, ACTIVITY_PARALLEL=3, LABEL=4, COMPONENT=5, NODE=6, ACTOR=7, USECASE=8, DECISION=9, MATRIX=10 };
	data_box::IType m_iType;
};

class data_pic
{
	public:
	QPixmap m_oPix;
	QPixmap m_oThumb;
	//QString m_sPicLocation; // keep them on the item?
	//QString m_sPicCaption;
	//QString m_sPicComment;
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
		//QPixmap m_oPix;
		//QPixmap m_oThumbnail;

		int m_iPicId; // references a data_pic

		QPixmap getPix();
		QPixmap getThumb();

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

		//bool load_from_path(const QString &);

		QHash<int, data_box*> m_oBoxes;
		QList<data_link*> m_oLinks;

		// transient
		int m_iObjectWidthHint;
		int m_iObjectHeightHint;
};

#endif

