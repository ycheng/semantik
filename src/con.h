// Thomas Nagy 2007-2012 GPLV3

#ifndef CON
#define CON

#include <QtGlobal>
#include <QHash>
#include <QVariant>
#include <QCoreApplication>

#define gratype(x) (QGraphicsItem::UserType + x)
#define CANVAS_ITEM_T gratype(12301)
#define CANVAS_FLAG_T gratype(12302)
#define CANVAS_LINK_T gratype(12303)
#define CANVAS_SORT_T gratype(12304)
#define BOX_ITEM_T gratype(12305)
#define BOX_LINK_T gratype(12306)

#define hash_params QHash<int, QVariant>

#define data_parent 0
#define data_id 1
#define data_id1 2
#define data_id2 3
#define data_orig 4
#define data_commande 5
#define data_type 6 // change the view stuff


#define VIEW_CANVAS 1
#define VIEW_LINEAR 2
#define VIEW_TEXT 3
#define VIEW_IMG 4
#define VIEW_DIAG 5
#define VIEW_TABLE 6


#define notr(x) QString(x)
#define xp(a, b, d) a->setItem(b, 0, new QStandardItem(d));
#define yp(a, b, d) a->setItem(b, 1, new QStandardItem(d));


#define GRID 10
#define nof(x, y) ((int) ((y) * ((int) ((x) / (y)))))
#define int_val2(x)  (nof(x, GRID/2.))
#define int_val(x)   (nof(x, GRID))


int grid_int(int x);



#define mu(x)   #x
#define fu(x, y)  mu(x##y)
#define moc(x) fu(x, moc)

#define min_offset 15

#define NO_ITEM 0
#define NO_VIEW 0

//#define EMPTY_TEXT "Empty"

//#define trUtf8(x) QCoreApplication::translate(NULL, x)
#incl\
ude \
"aux.h"


#endif

