// Thomas Nagy 2007-2012 GPLV3

#include <QMouseEvent>
#include <QAction>
#include <QGraphicsScene>
#include <QApplication>
#include<QList>
#include <QSet>
#include <QRegExp>
#include <QScrollBar>
#include <QMenu>
#include<QTextDocument>
#include <QColorDialog>
#include <QtDebug>
#include <QtDebug>
#include <QTextEdit>
#include <QKeyEvent>
#include <QScrollBar>
#include <QCompleter>
#include <QTreeView>
#include <QHeaderView>
#include <QAction>
#include <QStandardItemModel>

#include "aux.h"
#include "con.h"

#include <math.h>
#include "sem_mediator.h"
#include "box_item.h"
#include "box_dot.h"
#include "box_fork.h"
#include "box_link.h"
#include "data_item.h"
#include "box_view.h"
#include "sembind.h"
 #include "mem_box.h"

class box_reader : public QXmlDefaultHandler
{
    public:
	box_reader(box_view*);

	QString m_sBuf;
	int m_iVersion;
	box_view *m_oMediator;
	data_link *m_oCurrent;
	int m_iId;

	bool startElement(const QString&, const QString&, const QString&, const QXmlAttributes&);
	bool endElement(const QString&, const QString&, const QString&);
	bool characters(const QString &i_sStr);
};

box_reader::box_reader(box_view *i_oControl)
{
	m_oMediator = i_oControl;
	m_iId = m_oMediator->m_iId;
	m_oCurrent = NULL;
}

bool box_reader::startElement(const QString&, const QString&, const QString& i_sName, const QXmlAttributes& i_oAttrs)
{
	if (i_sName == QObject::trUtf8("box_item"))
	{
		int id = i_oAttrs.value(QObject::trUtf8("id")).toInt();

		data_box *box = new data_box(id);
		data_item *item = m_oMediator->m_oMediator->m_oItems[m_iId];
		m_oMediator->m_oMediator->m_oItems[m_iId]->m_oBoxes[id] = box;
		box->m_iXX = i_oAttrs.value(QObject::trUtf8("c1")).toFloat();
		box->m_iYY = i_oAttrs.value(QObject::trUtf8("c2")).toFloat();
		box->m_sText = i_oAttrs.value(QObject::trUtf8("text"));
		box->color = i_oAttrs.value(QObject::trUtf8("col"));
		//l_o->setRect(QRectF(0., 0., i_oAttrs.value(QObject::trUtf8("c3")).toDouble(), i_oAttrs.value(QObject::trUtf8("c4")).toDouble()));
	}
	else if (i_sName == QObject::trUtf8("box_link"))
	{
		data_link *link = new data_link();
		link->m_iParentPos = i_oAttrs.value(QObject::trUtf8("p1")).toInt();
		link->m_iParent    = i_oAttrs.value(QObject::trUtf8("c1")).toInt();
		link->m_iChildPos  = i_oAttrs.value(QObject::trUtf8("p2")).toInt();
		link->m_iChild     = i_oAttrs.value(QObject::trUtf8("c2")).toInt();

		if (link->m_iChildPos == 0) link->m_iChildPos = data_link::NORTH;
		if (link->m_iChildPos == 1) link->m_iChildPos = data_link::WEST;
		if (link->m_iChildPos == 2) link->m_iChildPos = data_link::SOUTH;
		if (link->m_iChildPos == 3) link->m_iChildPos = data_link::EAST;

		if (link->m_iParentPos == 0) link->m_iParentPos = data_link::NORTH;
		if (link->m_iParentPos == 1) link->m_iParentPos = data_link::WEST;
		if (link->m_iParentPos == 2) link->m_iParentPos = data_link::SOUTH;
		if (link->m_iParentPos == 3) link->m_iParentPos = data_link::EAST;

		link->pen_style = (Qt::PenStyle) i_oAttrs.value(QObject::trUtf8("sl")).toInt();
		link->border_width = i_oAttrs.value(QObject::trUtf8("wl")).toInt();
		m_oCurrent = link;
		m_oMediator->m_oMediator->m_oItems.value(m_iId)->m_oLinks.append(link);
	}
	else if (i_sName == QObject::trUtf8("box_link_offset"))
	{
		if (m_oCurrent)
		{
			m_oCurrent->m_oOffsets.append(QPoint(i_oAttrs.value(QObject::trUtf8("x")).toInt(), i_oAttrs.value(QObject::trUtf8("y")).toInt()));
		}
	}

	return true;
}

bool box_reader::endElement(const QString&, const QString&, const QString& i_sName)
{
	if (i_sName == QObject::trUtf8("box_link"))
	{
		m_oCurrent = NULL;
	}
	return true;
}

bool box_reader::characters(const QString &i_s)
{
	m_sBuf += i_s;
	return true;
}

box_view::box_view(QWidget *i_oWidget, sem_mediator *i_oControl) : QGraphicsView(i_oWidget)
{
	m_oMediator = i_oControl;
	m_iId = NO_ITEM;

	QGraphicsScene *l_oScene = new QGraphicsScene(this);
	l_oScene->setSceneRect(-400, -400, 400, 400);
	setScene(l_oScene);

	//setCacheMode(CacheBackground);
	setRenderHint(QPainter::Antialiasing);

	m_oEditAction = new QAction(QObject::trUtf8("Toggle edit"), this);
	m_oEditAction->setShortcut(QObject::trUtf8("Return"));
	connect(m_oEditAction, SIGNAL(triggered()), this, SLOT(slot_toggle_edit()));
	addAction(m_oEditAction);

	m_oCancelEditAction = new QAction(trUtf8("Cancel edit"), this);
	m_oCancelEditAction->setShortcut(notr("Escape"));
	addAction(m_oCancelEditAction);
	connect(m_oCancelEditAction, SIGNAL(triggered()), this, SLOT(slot_cancel_edit()));
	m_oCancelEditAction->setEnabled(false);

	m_oAddItemAction = new QAction(QObject::trUtf8("Add Box"), this);
	m_oAddItemAction->setShortcut(QObject::trUtf8("Ctrl+Return"));
	connect(m_oAddItemAction, SIGNAL(triggered()), this, SLOT(slot_add_item()));
	addAction(m_oAddItemAction);

	m_oDeleteAction = new QAction(QObject::trUtf8("Delete selection"), this);
	m_oDeleteAction->setShortcut(QObject::trUtf8("Delete"));
	connect(m_oDeleteAction, SIGNAL(triggered()), this, SLOT(slot_delete()));
	addAction(m_oDeleteAction);

	m_oColorAction = new QAction(QObject::trUtf8("Color..."), this);
	connect(m_oColorAction, SIGNAL(triggered()), this, SLOT(slot_color()));
	addAction(m_oColorAction);


	m_oMoveUpAction = new QAction(QObject::trUtf8("Move up"), this);
	m_oMoveUpAction->setShortcut(QObject::trUtf8("PgUp"));
	connect(m_oMoveUpAction, SIGNAL(triggered()), this, SLOT(slot_move_up()));
	addAction(m_oMoveUpAction);

	m_oMoveDownAction = new QAction(QObject::trUtf8("Move down"), this);
	m_oMoveDownAction->setShortcut(QObject::trUtf8("PgDown"));
	connect(m_oMoveDownAction, SIGNAL(triggered()), this, SLOT(slot_move_down()));
	addAction(m_oMoveDownAction);

	m_oAddDotStart = new QAction(QObject::trUtf8("Activity start"), this);
	connect(m_oAddDotStart, SIGNAL(triggered()), this, SLOT(slot_add_element()));
	m_oAddDotEnd = new QAction(QObject::trUtf8("Activity end"), this);
	connect(m_oAddDotEnd, SIGNAL(triggered()), this, SLOT(slot_add_element()));
	m_oAddParallelHorizontal = new QAction(QObject::trUtf8("Horizontal fork/join"), this);
	connect(m_oAddParallelHorizontal, SIGNAL(triggered()), this, SLOT(slot_add_element()));
	m_oAddParallelVertical = new QAction(QObject::trUtf8("Vertical fork/join"), this);
	connect(m_oAddParallelVertical, SIGNAL(triggered()), this, SLOT(slot_add_element()));

	m_oMenu = new QMenu(this);
	m_oMenu->addAction(m_oAddItemAction);

	m_oAddBoxMenu = m_oMenu->addMenu(QObject::trUtf8("Add element"));
	m_oAddBoxMenu->addAction(m_oAddDotStart);
	m_oAddBoxMenu->addAction(m_oAddDotEnd);
	m_oAddBoxMenu->addAction(m_oAddParallelHorizontal);
	m_oAddBoxMenu->addAction(m_oAddParallelVertical);

	m_oMenu->addAction(m_oEditAction);
	m_oMenu->addAction(m_oDeleteAction);
	m_oMenu->addAction(m_oColorAction);
	//m_oMenu->addAction(m_oMoveUpAction);
	//m_oMenu->addAction(m_oMoveDownAction);

	QAction *l_o;

	m_oWidthMenu = m_oMenu->addMenu(QObject::trUtf8("Connection width"));
	m_oWidthGroup = new QActionGroup(this);
	l_o = m_oWidthMenu->addAction(QObject::trUtf8("1px")); connect(l_o, SIGNAL(triggered()), this, SLOT(slot_penwidth())); addAction(l_o); l_o->setData(QVariant(1)); m_oWidthGroup->addAction(l_o);
	l_o = m_oWidthMenu->addAction(QObject::trUtf8("2px")); connect(l_o, SIGNAL(triggered()), this, SLOT(slot_penwidth())); addAction(l_o); l_o->setData(QVariant(2)); m_oWidthGroup->addAction(l_o);
	l_o = m_oWidthMenu->addAction(QObject::trUtf8("3px")); connect(l_o, SIGNAL(triggered()), this, SLOT(slot_penwidth())); addAction(l_o); l_o->setData(QVariant(3)); m_oWidthGroup->addAction(l_o);

	m_oStyleMenu = m_oMenu->addMenu(QObject::trUtf8("Connection style"));
	m_oStyleGroup = new QActionGroup(this);
	l_o = m_oStyleMenu->addAction(QObject::trUtf8("solid line")); connect(l_o, SIGNAL(triggered()), this, SLOT(slot_penstyle())); addAction(l_o); l_o->setData(QVariant(Qt::SolidLine)); m_oStyleGroup->addAction(l_o);
	l_o = m_oStyleMenu->addAction(QObject::trUtf8("dot line")); connect(l_o, SIGNAL(triggered()), this, SLOT(slot_penstyle())); addAction(l_o); l_o->setData(QVariant(Qt::DotLine)); m_oStyleGroup->addAction(l_o);
	l_o = m_oStyleMenu->addAction(QObject::trUtf8("dash line")); connect(l_o, SIGNAL(triggered()), this, SLOT(slot_penstyle())); addAction(l_o); l_o->setData(QVariant(Qt::DashLine)); m_oStyleGroup->addAction(l_o);


	m_oAddItemAction->setEnabled(false);
	m_oDeleteAction->setEnabled(false);

	m_oEditAction->setEnabled(false);
	//m_oMoveUpAction->setEnabled(false);
	//m_oMoveDownAction->setEnabled(false);
	m_oMenu->addSeparator();
	m_oColorAction->setEnabled(false);

	m_oLastMovePoint = QPointF(-100, -100);

	m_oCurrent = NULL;
	m_bPressed = false;
	m_bScroll = false;

	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
}

box_view::~box_view()
{

}

void box_view::notify_add_item(int id)
{
	m_iId = id;
	setEnabled(true);
}

void box_view::notify_edit_box(int id, int bid)
{
	Q_ASSERT(id == m_iId);
	box_item *item = (box_item*) m_oItems.value(bid); // TODO
	Q_ASSERT(item != NULL);
	data_box *box = item->m_oBox;
	if (box->m_sText != item->toPlainText())
	{
		item->setPlainText(box->m_sText);
	}
}

void box_view::notify_select(const QList<int>& unsel, const QList<int>& sel)
{
	clear_diagram();

	if (sel.size() != 1)
	{
		m_iId = NO_ITEM;
		setEnabled(false);
		m_oEditAction->setEnabled(false);
		m_oCancelEditAction->setEnabled(false);
	}
	else
	{
		m_iId = sel.at(0);
		data_item *l_oData = m_oMediator->m_oItems.value(m_iId);
		Q_ASSERT(l_oData);
		if (l_oData and l_oData->m_iDataType == VIEW_DIAG)
		{
			if (!l_oData->m_sDiag.isEmpty())
			{
				from_string(l_oData->m_sDiag);
				l_oData->m_sDiag = "";
			}
			sync_view();
			check_canvas_size();
		}
		setEnabled(true);
	}
}

void box_view::sync_view()
{
	if (!m_iId) return;
	
	data_item *item = m_oMediator->m_oItems.value(m_iId);
	Q_ASSERT(item);
	if (item->m_iDataType != VIEW_DIAG)
	{
		return;
	}
	if (!item->m_sDiag.isEmpty())
	{
		from_string(item->m_sDiag);
		item->m_sDiag = "";
	}

	foreach (data_box *box, item->m_oBoxes.values())
	{
		if (box->m_iType == data_box::TEXT)
		{
			box_item *l_o = new box_item(this, box->m_iId);
			m_oItems[box->m_iId] = l_o;
			l_o->setPlainText(box->m_sText);
			l_o->setPos(QPointF(box->m_iXX, box->m_iYY));
			l_o->update_data();
		}
		else if (box->m_iType == data_box::ACTIVITY_START)
		{
			box_dot *l_o = new box_dot(this, box->m_iId);
			m_oItems[box->m_iId] = l_o;
			l_o->setPos(QPointF(box->m_iXX, box->m_iYY));
			l_o->setRect(QRectF(QPointF(0, 0), QSizeF(20, 20)));
			l_o->update_data();
		}
		else if (box->m_iType == data_box::ACTIVITY_PARALLEL)
		{
			box_fork *l_o = new box_fork(this, box->m_iId);
			m_oItems[box->m_iId] = l_o;
			l_o->setPos(QPointF(box->m_iXX, box->m_iYY));
			l_o->update_data();
		}
		else
		{
			Q_ASSERT(false);
		}
	}

	foreach (data_link *link, item->m_oLinks) {
		box_link *l_o = new box_link(this);
		l_o->m_oInnerLink.copy_from(*link);
		l_o->m_oLink = link;
		l_o->update_pos();
		m_oLinks.append(l_o);
	}
}

void box_view::notify_export_item(int id)
{
	int l_iOldId = m_iId;
	if (m_oMediator->m_oItems.value(id)->m_iDataType != VIEW_DIAG)
		return;
	clear_diagram();

	m_iId = id;
	sync_view();

	QRectF l_oRect = scene()->itemsBoundingRect();
	l_oRect = QRectF(l_oRect.topLeft() - QPointF(25, 25), l_oRect.bottomRight() + QPointF(25, 25));

	QRectF l_oR(0, 0, l_oRect.width(), l_oRect.height());

	QPair<int, int> p = m_oMediator->hint_size_diagram(id);

	if (p.first != 0) {
		l_oR.setWidth(p.first);
		if (p.second == 0) {
			l_oR.setHeight((p.first * l_oRect.height()) / (double) l_oRect.width());
		}
	}
	if (p.second != 0) {
		l_oR.setHeight(p.second);
		if (p.first == 0) {
			l_oR.setWidth((p.second * l_oRect.width()) / (double) l_oRect.height());
		}
	}

	// fill with white
	QImage l_oImage((int) l_oR.width(), (int) l_oR.height(), QImage::Format_RGB32);
	l_oImage.fill(qRgb(255,255,255));

	QPainter l_oP;
	l_oP.begin(&l_oImage);
	l_oP.setRenderHints(QPainter::Antialiasing);
	Qt::AspectRatioMode rat = (p.first == 0 || p.second == 0) ? Qt::KeepAspectRatio : Qt::IgnoreAspectRatio;
	scene()->render(&l_oP, l_oR, l_oRect, rat);
	l_oP.end();

	l_oImage.save(QString(m_oMediator->m_sTempDir + QString("/") + QString("diag-%1.png")).arg(QString::number(m_iId)));
	clear_diagram();
	m_iId = l_iOldId;
	if (m_iId != NO_ITEM)
	{
		sync_view();
	}
}

void box_view::slot_delete()
{
	QList<data_box*> boxes;
	QSet<data_link*> links;
	foreach (QGraphicsItem* el, scene()->selectedItems()) {
		if (box_item *bit = dynamic_cast<box_item*>(el)) {
			boxes.append(bit->m_oBox);
			foreach (box_link* l, m_oLinks)
			{
				if (l->m_oLink->m_iParent == bit->m_oBox->m_iId || l->m_oLink->m_iChild == bit->m_oBox->m_iId)
				{
					links << l->m_oLink;
				}
			}

		} else if (box_link *l = dynamic_cast<box_link*>(el)) {
			links << l->m_oLink;
			Q_ASSERT(l->m_oLink);
		}
	}

	if (boxes.size() > 0 || links.size() > 0) {
		mem_del_box *del = new mem_del_box(m_oMediator, m_iId);
		del->init(boxes, links.toList());
		del->apply();
	}
}

void box_view::enable_menu_actions()
{
	QList<QGraphicsItem*> selection = scene()->selectedItems();
	int selected = selection.size();

	m_oAddItemAction->setEnabled(selected <= 1);
	m_oDeleteAction->setEnabled(selected >= 1);
	m_oColorAction->setEnabled(selected >= 1);

	m_oEditAction->setEnabled(selected == 1 and dynamic_cast<box_item*>(selection.at(0)));

	m_oWidthMenu->setEnabled(selected >= 1 and dynamic_cast<box_link*>(selection.at(0)));
	foreach(QAction* l_o, m_oWidthGroup->actions())
	{
		l_o->setEnabled(selected >= 1);
	}

	m_oStyleMenu->setEnabled(selected >= 1 and dynamic_cast<box_link*>(selection.at(0)));
	foreach(QAction* l_o, m_oStyleGroup->actions())
	{
		l_o->setEnabled(selected >= 1);
	}
}

void box_view::enable_actions()
{
	foreach (QAction* l_o, actions())
	{
		l_o->setEnabled(true); // TODO we could disable the move actions one by one
	}
	enable_menu_actions();
}

void box_view::slot_add_item()
{
	foreach(QGraphicsItem *l_o, scene()->items()) {
		l_o->setSelected(false);
	}

	mem_add_box *add = new mem_add_box(m_oMediator, m_iId, next_seq());
	add->box->m_iXX = m_oLastPoint.x();
	add->box->m_iYY = m_oLastPoint.y();
	add->apply();

	box_item *l_o = (box_item*) m_oItems.value(add->box->m_iId);
	l_o->setSelected(true);
	// l_o->setPos(m_oLastPoint - QPointF(l_oRect.width()/2, l_oRect.height()/2)); // TODO
}

void box_view::change_colors(QAction* i_oAct)
{
	if (!hasFocus()) return;
	if (scene()->selectedItems().size() < 1) return;

	QColor l_oColor;
	static QColor selected_color;

	for (int i=1; i < i_oAct->actionGroup()->actions().size(); ++i)
	{
		if (i_oAct->actionGroup()->actions()[i] == i_oAct)
		{
			if (i == i_oAct->actionGroup()->actions().size()-1)
			{
				selected_color = QColorDialog::getColor(selected_color, this);
				if (!selected_color.isValid())
				{
					return;
				}
				l_oColor = selected_color;
			}
			else
			{
				selected_color = l_oColor = m_oMediator->m_oColorSchemes[i].m_oInnerColor;

			}
			break;
		}
	}

	mem_prop_box *mem = new mem_prop_box(m_oMediator, m_iId);
	foreach (QGraphicsItem *l_o, scene()->selectedItems())
	{
		if (box_link *k = dynamic_cast<box_link*>(l_o))
		{
			mem->items.append(((box_link*) l_o)->m_oLink);
		}
		else if (connectable* t = dynamic_cast<connectable*>(l_o))
		{
			mem->items.append(t->m_oBox);
		}
	}
	mem->change_type = CH_COLOR;
	mem->new_props.color = l_oColor;
	mem->apply();
}

void box_view::slot_color()
{
	if (scene()->selectedItems().size() < 1) return;
	QColor l_oColor = QColorDialog::getColor(l_oColor, this);
	if (!l_oColor.isValid()) return;
	mem_prop_box *mem = new mem_prop_box(m_oMediator, m_iId);
	foreach (QGraphicsItem *l_o, scene()->selectedItems())
	{
		if (box_link *k = dynamic_cast<box_link*>(l_o))
		{
			mem->items.append(k->m_oLink);
		}
		else if (connectable *k = dynamic_cast<connectable*>(l_o))
		{
			mem->items.append(k->m_oBox);
		}
	}
	mem->change_type = CH_COLOR;
	mem->new_props.color = l_oColor;
	mem->apply();
}

void box_view::slot_penstyle()
{
	int l_i = ((QAction*) QObject::sender())->data().toInt();
	mem_prop_box *mem = new mem_prop_box(m_oMediator, m_iId);
	foreach (QGraphicsItem* l_o, scene()->selectedItems())
	{
		if (box_link *k = dynamic_cast<box_link*>(l_o))
		{
			mem->items.append(k->m_oLink);
		}
	}
	mem->change_type = CH_PENST;
	mem->new_props.pen_style = (Qt::PenStyle) l_i;
	mem->apply();
}

void box_view::slot_add_element()
{
	QAction *sender = (QAction*) QObject::sender();
	mem_add_box *add = new mem_add_box(m_oMediator, m_iId, next_seq());
	add->box->m_iXX = m_oLastPoint.x();
	add->box->m_iYY = m_oLastPoint.y();

	add->box->m_bIsVertical = sender == m_oAddParallelVertical;
	add->box->m_bIsEnd = sender == m_oAddDotEnd;
	
	if (sender == m_oAddDotEnd || sender == m_oAddDotStart) {
		add->box->m_iType = data_box::ACTIVITY_START;
		add->box->color = QColor(Qt::black);
	}
	if (sender == m_oAddParallelHorizontal || sender == m_oAddParallelVertical) {
		add->box->m_iType = data_box::ACTIVITY_PARALLEL;
		add->box->color = QColor(Qt::black);
	}
	add->apply();

	QGraphicsItem *l_o = dynamic_cast<QGraphicsItem*>(m_oItems.value(add->box->m_iId));
	l_o->setSelected(true);
}

void box_view::slot_penwidth()
{
	int l_i = ((QAction*) QObject::sender())->data().toInt();
	mem_prop_box *mem = new mem_prop_box(m_oMediator, m_iId);
	foreach (QGraphicsItem* l_o, scene()->selectedItems())
	{
		if (box_link *k = dynamic_cast<box_link*>(l_o))
		{
			mem->items.append(k->m_oLink);
		}
	}
	mem->change_type = CH_BORDER;
	mem->new_props.border_width = l_i;
	mem->apply();
}

void box_view::slot_toggle_edit()
{
	if (!hasFocus()) {
		return;
	}

	box_item* sel = NULL;
	foreach (QGraphicsItem *tmp, items())
	{
		box_item* k;
		if (tmp->isSelected() && (k = dynamic_cast<box_item*>(tmp)))
		{
			if (sel) {
				sel = NULL;
				break;
			} else {
				sel = k;
			}
		}
	}

	if (sel) {
		if (sel->textInteractionFlags() & Qt::TextEditorInteraction) {
			sel->setTextInteractionFlags(Qt::NoTextInteraction);
			/*if (sel->toPlainText() == QObject::trUtf8("")) {
				sel->setPlainText(QObject::trUtf8("Empty"));
				sel->update(); // seems to update faster
				//sel->update_links();

			}*/
			if (sel->toPlainText() != m_oMediator->m_oItems[m_iId]->m_oBoxes[sel->m_iId]->m_sText) {
				mem_edit_box *ed = new mem_edit_box(m_oMediator, m_iId, sel->m_iId);
				ed->newText = sel->toPlainText();
				ed->apply();
			}
		} else {
			sel->setTextInteractionFlags(Qt::TextEditorInteraction);
			sel->setFocus();

			m_oAddItemAction->setEnabled(false);
			m_oDeleteAction->setEnabled(false);
			m_oCancelEditAction->setEnabled(true);

			/*
			m_oInsertSiblingAction->setEnabled(false);
			m_oNextRootAction->setEnabled(false);

			m_oMoveUpAction->setEnabled(false);
			m_oMoveDownAction->setEnabled(false);
			m_oMoveLeftAction->setEnabled(false);
			m_oMoveRightAction->setEnabled(false);
			m_oSelectUpAction->setEnabled(false);
			m_oSelectDownAction->setEnabled(false);
			m_oSelectLeftAction->setEnabled(false);
			m_oSelectRightAction->setEnabled(false);
			m_oMediator->check_undo(false);
			*/
			return;
		}
	}

	m_oAddItemAction->setEnabled(true);
	m_oDeleteAction->setEnabled(true);
	m_oCancelEditAction->setEnabled(false);

	/*
	m_oInsertSiblingAction->setEnabled(true);
	m_oNextRootAction->setEnabled(true);

	m_oMoveUpAction->setEnabled(true);
	m_oMoveDownAction->setEnabled(true);
	m_oMoveLeftAction->setEnabled(true);
	m_oMoveRightAction->setEnabled(true);
	m_oSelectUpAction->setEnabled(true);
	m_oSelectDownAction->setEnabled(true);
	m_oSelectLeftAction->setEnabled(true);
	m_oSelectRightAction->setEnabled(true);
	m_oMediator->check_undo(true);
	*/
}

void box_view::slot_cancel_edit()
{
	//qDebug()<<"slot cancel edit";
	box_item* sel = NULL;
	foreach (QGraphicsItem *tmp, items()) {
		box_item *k;
		if (tmp->isSelected() && (k = dynamic_cast<box_item*>(tmp)))
		{
			if (sel) {
				sel = NULL;
				break;
			} else {
				sel = k;
			}
		}
	}

	if (sel && sel->textInteractionFlags() & Qt::TextEditorInteraction) {
		//qDebug()<<"cancel edit!"<<sel;
		sel->setTextInteractionFlags(Qt::NoTextInteraction);
		sel->update_data();
	}

	m_oAddItemAction->setEnabled(true);
	m_oDeleteAction->setEnabled(true);
}

void box_view::slot_move_up()
{
	foreach (QGraphicsItem *l_oItem, scene()->selectedItems())
	{
		l_oItem->setZValue(l_oItem->zValue()+1);
	}
}

void box_view::slot_move_down()
{
	foreach (QGraphicsItem *l_oItem, scene()->selectedItems())
	{
		l_oItem->setZValue(l_oItem->zValue()-1);
	}
}

void box_view::from_string(const QString &i_s)
{
	box_reader l_oHandler(this);
	QXmlInputSource l_oSource;
	l_oSource.setData(i_s);
	QXmlSimpleReader l_oReader;
	l_oReader.setContentHandler(&l_oHandler);
	if (!l_oReader.parse(l_oSource))
	{
		//qDebug()<<"parse error!!";
		clear_diagram();
	}
}

void box_view::clear_diagram()
{
	foreach (box_link *l_o, m_oLinks) {
		scene()->removeItem(l_o);
		delete l_o;
	}
	m_oLinks.clear();
	foreach (connectable *l_o, m_oItems.values()) {
		scene()->removeItem(dynamic_cast<QGraphicsItem*>(l_o));
		delete l_o;
	}
	m_oItems.clear();
}

void box_view::check_canvas_size()
{
	QRect l_oRect = viewport()->rect();
	if (m_oItems.size() < 1) // no rectangle
	{
		scene()->setSceneRect(QRectF(mapToScene(l_oRect.topLeft()), mapToScene(l_oRect.bottomRight())));
		return;
	}

	QRectF br = scene()->itemsBoundingRect();
	br.adjust(-GAP, -GAP, GAP, GAP);

	QRectF sc = br.united(QRectF(mapToScene(l_oRect.topLeft()), mapToScene(l_oRect.bottomRight())));
	if (sc == sceneRect()) return;
	scene()->setSceneRect(sc);
}

void box_view::focusInEvent(QFocusEvent *i_oEv)
{
	QGraphicsView::focusInEvent(i_oEv);
	m_oMediator->notify_focus(this);
}

void box_view::focusOutEvent(QFocusEvent *i_oEv)
{
	QGraphicsView::focusOutEvent(i_oEv);
	edit_off();
}

void box_view::notify_add_box(int id, int box)
{
	Q_ASSERT(m_iId == id);
	data_item *item = m_oMediator->m_oItems.value(m_iId);
	data_box *db = item->m_oBoxes[box];
	connectable *l_o = NULL;
	if (db->m_iType == data_box::TEXT)
	{
		l_o = new box_item(this, box);
	}
	else if (db->m_iType == data_box::ACTIVITY_START)
	{
		box_dot *tmp = new box_dot(this, box);
		tmp->setRect(QRectF(QPointF(0, 0), QSizeF(20, 20)));
		l_o = tmp;
	}
	else if (db->m_iType == data_box::ACTIVITY_PARALLEL)
	{
		if (db->m_bIsVertical)
		{
			db->m_iHH = FORK_LENGTH;
			db->m_iWW = FORK_WIDTH;
		}
		else
		{
			db->m_iWW = FORK_LENGTH;
			db->m_iHH = FORK_WIDTH;
		}
		l_o = new box_fork(this, box);
	}
	Q_ASSERT(l_o != NULL);
	m_oItems[box] = l_o;
	l_o->update_data();
}

void box_view::notify_del_box(int id, int box)
{
	connectable *l_o = m_oItems.value(box);
        Q_ASSERT(l_o!=NULL);
        scene()->removeItem(dynamic_cast<QGraphicsItem*>(l_o));
	m_oItems.remove(box);
	delete l_o;
}

void box_view::notify_link_box(int id, data_link* link)
{
	box_link *l_o = m_oCurrent;
	if (!l_o)
	{
		l_o = new box_link(this);
	}
	m_oLinks.push_back(l_o);
	l_o->m_oInnerLink.copy_from(*link);
	l_o->m_oLink = link;
	l_o->update_pos();
}

void box_view::notify_unlink_box(int id, data_link* link)
{
	Q_ASSERT(!m_oCurrent);
	foreach (box_link *cur, m_oLinks) {
		if (cur->m_oLink == link)
		{
			delete cur;
			m_oLinks.removeAll(cur);
			break;
		}
	}
}

void box_view::notify_change_link_box(int id, data_link*link)
{
	foreach (box_link *cur, m_oLinks)
	{
		if (cur->m_oLink == link)
		{
			cur->m_oInnerLink.copy_from(*cur->m_oLink);
			cur->update_pos();
			break;
		}
	}
}

void box_view::wheelEvent(QWheelEvent *i_oEvent)
{
	QPointF l_o = mapToScene(i_oEvent->pos());
	qreal i_iScaleFactor = pow(2.0, i_oEvent->delta() / 440.0);
	qreal i_rFactor = matrix().scale(i_iScaleFactor, i_iScaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
	if (i_rFactor < 0.01 || i_rFactor > 1000) return;
	scale(i_iScaleFactor, i_iScaleFactor);
	centerOn(l_o + mapToScene(viewport()->rect().center()) - mapToScene(i_oEvent->pos()));
	check_canvas_size();
}

void box_view::keyPressEvent(QKeyEvent *i_oEvent)
{
	QGraphicsView::keyPressEvent(i_oEvent);
	if (QApplication::keyboardModifiers() & Qt::ShiftModifier) setCursor(Qt::CrossCursor);
}

void box_view::keyReleaseEvent(QKeyEvent *i_oEvent)
{
	QGraphicsView::keyReleaseEvent(i_oEvent);
	setCursor(Qt::ArrowCursor);
}

void box_view::mouseDoubleClickEvent(QMouseEvent* i_oEv)
{
	if (i_oEv->button() != Qt::LeftButton)
	{
		//qDebug()<<"left button on click, leaving";
		return;
	}

	QPointF m_oLastPoint = mapToScene(i_oEv->pos());
	QGraphicsItem *l_oItem = itemAt(i_oEv->pos());
	box_link *l_oLink;
	if (l_oItem && (l_oLink = dynamic_cast<box_link*>(l_oItem)))
	{
		mem_unlink_box *rm = new mem_unlink_box(m_oMediator, m_iId);
		rm->link = l_oLink->m_oLink;
		rm->apply();

		return;
	}

	if (i_oEv->modifiers() != Qt::ShiftModifier) {
		//qDebug()<<"adding a box from double click";

		mem_add_box *add = new mem_add_box(m_oMediator, m_iId, next_seq());
		add->box->m_iXX = m_oLastPoint.x();
		add->box->m_iYY = m_oLastPoint.y();
		add->apply();

		QGraphicsItem *l_o = dynamic_cast<QGraphicsItem*>(m_oItems.value(add->box->m_iId));
		l_o->setSelected(true);
	}
}

void box_view::mousePressEvent(QMouseEvent *i_oEv)
{
	if (i_oEv->button() == Qt::RightButton)
	{
		edit_off();
		m_oLastPoint = mapToScene(i_oEv->pos());
		enable_menu_actions();
		m_oMenu->popup(i_oEv->globalPos());
		return;
	}

	m_bPressed = true;
	m_oLastMovePoint = mapToScene(i_oEv->pos());
	m_oLastPoint = mapToScene(i_oEv->pos());

	if (i_oEv->button() == Qt::MidButton)
	{
		m_oScrollPoint = i_oEv->pos();
		viewport()->setCursor(Qt::OpenHandCursor);
		m_bScroll = true;
		return;
	}

	QGraphicsItem *l_oItem = scene()->itemAt(mapToScene(i_oEv->pos()));

	connectable* kk;
	if (l_oItem && (kk = dynamic_cast<connectable*>(l_oItem)))
	{
		if (QApplication::keyboardModifiers() & Qt::ShiftModifier)
		{
			if (m_oCurrent) return;

			foreach (QGraphicsItem *l_o, scene()->selectedItems())
			{
				l_o->setSelected(false);
			}

			QPoint p = QPoint(m_oLastPoint.x(), m_oLastPoint.y());

			m_oCurrent = new box_link(this);
			m_oCurrent->m_oInnerLink.m_iParent = kk->m_iId;
			m_oCurrent->m_oInnerLink.m_iParentPos = kk->choose_position(m_oLastMovePoint);
			m_oCurrent->m_oInnerLink.m_iChild = NO_ITEM;
			m_oCurrent->m_oInnerLink.m_iChildPos = data_link::NORTH;
			m_oCurrent->m_oInnerLink.m_oStartPoint = m_oCurrent->m_oInnerLink.m_oEndPoint = p;

			mem_link_box *ln = new mem_link_box(m_oMediator, m_iId);
			ln->link = new data_link();
			ln->link->copy_from(m_oCurrent->m_oInnerLink);
			ln->apply();

			l_oItem->setSelected(false);
			m_oCurrent->setSelected(true);
		}
	}

	box_item *k;
	if (l_oItem && (k = dynamic_cast<box_item*>(l_oItem)))
	{
		QPointF l_o = l_oItem->pos();
		// TODO used by the handle for resizing the boxes - check that the click was on the handle
		if (m_oLastPoint.x() + m_oLastPoint.y() - l_o.x() - l_o.y() >
			k->rect().width() + k->rect().height() - 2*GRID_VALUE)
		{
			m_oOffsetPoint = QPointF(k->rect().width(), k->rect().height());
		}
		else
		{
			m_oOffsetPoint = QPointF(-1, -1);
		}
	}

	QGraphicsView::mousePressEvent(i_oEv);
	edit_off();
}

void box_view::mouseMoveEvent(QMouseEvent *i_oEv)
{
	if (m_bScroll)
	{
		QScrollBar *l_h_bar = horizontalScrollBar();
		QScrollBar *l_v_bar = verticalScrollBar();

		QPoint l_o = i_oEv->pos() - m_oScrollPoint;

		l_h_bar->setValue(l_h_bar->value() + (isRightToLeft() ? l_o.x() : - l_o.x()));
		l_v_bar->setValue(l_v_bar->value() - l_o.y());

		m_oScrollPoint = i_oEv->pos();
		return;
	}

	if (!m_bPressed)
	{
		return;
	}

	m_oLastMovePoint = mapToScene(i_oEv->pos());
	QGraphicsView::mouseMoveEvent(i_oEv);
}

void box_view::mouseReleaseEvent(QMouseEvent *i_oEv)
{
	if (m_bScroll)
	{
		QGraphicsView::mouseReleaseEvent(i_oEv);
		m_bPressed = false;
		m_bScroll = false;
		viewport()->setCursor(Qt::ArrowCursor);
		return;
	}

	QGraphicsView::mouseReleaseEvent(i_oEv);

	m_bPressed = false;
	{
		QPointF p = mapToScene(i_oEv->pos()) - m_oLastPoint;
		if (qAbs(p.x()) > 1 || qAbs(p.y()) > 1)
		{
			mem_pos_box *mem = new mem_pos_box(m_oMediator, m_iId);
			foreach (QGraphicsItem *l_oI1, scene()->selectedItems())
			{
				if (connectable* item = dynamic_cast<connectable*>(l_oI1))
				{
					data_box *box = item->m_oBox;
					mem->prev_values[box] = QPointF(box->m_iXX, box->m_iYY);
					mem->next_values[box] = l_oI1->pos();
				}
			}
			if (mem->prev_values.size() > 0)
			{
				mem->apply();
			}
			else
			{
				delete mem;
			}
		}
	}
	check_canvas_size();
}

void box_view::edit_off() {
	box_item* sel = NULL;
	foreach (QGraphicsItem *tmp, items()) {
		if (sel = dynamic_cast<box_item*>(tmp))
		{
			sel = (box_item*) tmp;
			if (sel->textInteractionFlags() & Qt::TextEditorInteraction)
			{
				sel->setTextInteractionFlags(Qt::NoTextInteraction);
				if (sel->toPlainText() != m_oMediator->m_oItems[m_iId]->m_oBoxes[sel->m_iId]->m_sText) {
					mem_edit_box *ed = new mem_edit_box(m_oMediator, m_iId, sel->m_iId);
					ed->newText = sel->toPlainText();
					ed->apply();
				}
				m_oAddItemAction->setEnabled(true);
				m_oDeleteAction->setEnabled(true);
				m_oCancelEditAction->setEnabled(false);
			}
		}
	}
}

void box_view::notify_box_props(int id, const QList<diagram_item*>& items)
{
	Q_ASSERT(id == m_iId);
	foreach (diagram_item* it, items) {
		foreach (box_link *lnk, m_oLinks)
		{
			if (lnk->m_oLink == it) {
				lnk->update();
				goto end;
			}
		}

		foreach (connectable *box, m_oItems.values())
		{
			if (box->m_oBox == it) {
				(dynamic_cast<QGraphicsItem*>(box))->update();
				goto end;
			}
		}
		end:
			;
	}
}

void box_view::notify_pos_box(int id, const QList<data_box*>& items)
{
	Q_ASSERT(id == m_iId);
	foreach (data_box *box, items)
	{
		m_oItems[box->m_iId]->update_data();
	}
}

int box_view::next_seq()
{
	do {
		++num_seq;
	} while (m_oItems.contains(num_seq));
	return num_seq;
}

void box_view::notify_focus(void* ptr)
{
	bool cond = ptr == this;
	m_oAddItemAction->setEnabled(cond);
	m_oDeleteAction->setEnabled(cond);
	m_oEditAction->setEnabled(cond);
}

#include "box_view.moc"

