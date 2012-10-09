// Thomas Nagy 2007-2012 GPLV3

#include "canvas_flag.h"
#include "data_item.h"
#include  <QtGlobal>
#include "sem_mediator.h"
#include "canvas_view.h"
#include  <QMenu> 
#include <QToolTip>
#include  <QColorDialog> 
#include<KToolBar> 
#include<KMessageBox>
#include <KDialog>
#include <KUrl>
#include  <QActionGroup> 
#include "canvas_item.h"
#include "canvas_link.h"
#include<QCoreApplication>
#include <QSet>
#include "canvas_sort.h"
#include <QRadioButton>
#include "canvas_sort_toggle.h"
#include "semantik.h"
#include <QTextCursor> 
#include  <QApplication> 
#include "canvas_chain.h"
#include <QtDebug>
#include  <QX11Info>
#include <QScrollBar>
#include  <QGraphicsScene>
#include <QWheelEvent>
#include  <QMatrix>
#include <QPointF>
#include <KFileDialog>
#include <QSpinBox>
#include "export_map_dialog.h"
#include "kurlrequester.h"

 #include <QGraphicsTextItem>

#include <math.h>

canvas_view::canvas_view(QWidget *i_oWidget, sem_mediator *i_oControl) : QGraphicsView(i_oWidget)
{
	m_oSemantikWindow = i_oWidget;
	m_iSortId = NO_ITEM;
	m_iSortCursor = 0;

	m_oLastPoint = QPointF(0, 0);

	//m_oRubbery = new QRubberBand(QRubberBand::Rectangle, this);
	//m_oRubbery->setGeometry(QRect(0, 0, 0, 0));

	QGraphicsScene *l_oScene = new QGraphicsScene(this);
	l_oScene->setSceneRect(-400, -400, 400, 400);
	setScene(l_oScene);

	//setCacheMode(CacheBackground);
	setRenderHint(QPainter::Antialiasing);

	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

	setMinimumSize(200, 200);

	m_oMediator = i_oControl;

	m_bPressed = false;
	m_bScroll = false;

	m_oRubberLine = new rubber_line(QRubberBand::Line, this);

	QBrush l_oBrush = QBrush();
	//QColor l_oColor = QColor("#000077");
	//l_oColor.setAlpha(200);

	QColor l_oColor = QColor(0, 0, 150, 100);
	l_oBrush.setColor(l_oColor);
	//m_oRubber->setBrush(l_oBrush);

	QAction *l_o = NULL;

	m_oAddItemAction = new QAction(trUtf8("Insert child"), this);
	m_oAddItemAction->setShortcut(notr("Ctrl+Return"));
	connect(m_oAddItemAction, SIGNAL(triggered()), this, SLOT(slot_add_item()));
	addAction(m_oAddItemAction);

	m_oDeleteAction = new QAction(trUtf8("Delete selection"), this);
	m_oDeleteAction->setShortcut(notr("Del"));
	connect(m_oDeleteAction, SIGNAL(triggered()), this, SLOT(slot_delete()));
	addAction(m_oDeleteAction);

	m_oInsertSiblingAction = l_o = new QAction(trUtf8("Insert sibling"), this);
	l_o->setShortcut(notr("Shift+Return"));
	connect(l_o, SIGNAL(triggered()), this, SLOT(slot_add_sibling()));
	addAction(l_o);

	m_oMoveUpAction = l_o = new QAction(trUtf8("Move up"), this); l_o->setShortcut(notr("Alt+Up")); connect(l_o, SIGNAL(triggered()), this, SLOT(slot_move())); addAction(l_o); l_o->setData(QVariant(0));
	m_oMoveDownAction = l_o = new QAction(trUtf8("Move down"), this); l_o->setShortcut(notr("Alt+Down")); connect(l_o, SIGNAL(triggered()), this, SLOT(slot_move())); addAction(l_o); l_o->setData(QVariant(1));
	m_oMoveLeftAction = l_o = new QAction(trUtf8("Move left"), this); l_o->setShortcut(notr("Alt+Left")); connect(l_o, SIGNAL(triggered()), this, SLOT(slot_move())); addAction(l_o); l_o->setData(QVariant(2));
	m_oMoveRightAction = l_o = new QAction(trUtf8("Move right"), this); l_o->setShortcut(notr("Alt+Right")); connect(l_o, SIGNAL(triggered()), this, SLOT(slot_move())); addAction(l_o); l_o->setData(QVariant(3));

	m_oSelectUpAction = l_o = new QAction(trUtf8("Select up"), this); l_o->setShortcut(notr("Up")); connect(l_o, SIGNAL(triggered()), this, SLOT(slot_sel())); addAction(l_o); l_o->setData(QVariant(0));
	m_oSelectDownAction = l_o = new QAction(trUtf8("Select down"), this); l_o->setShortcut(notr("Down")); connect(l_o, SIGNAL(triggered()), this, SLOT(slot_sel())); addAction(l_o); l_o->setData(QVariant(1));
	m_oSelectLeftAction = l_o = new QAction(trUtf8("Select left"), this); l_o->setShortcut(notr("Left")); connect(l_o, SIGNAL(triggered()), this, SLOT(slot_sel())); addAction(l_o); l_o->setData(QVariant(2));
	m_oSelectRightAction = l_o = new QAction(trUtf8("Select right"), this); l_o->setShortcut(notr("Right")); connect(l_o, SIGNAL(triggered()), this, SLOT(slot_sel())); addAction(l_o); l_o->setData(QVariant(3));


	m_oSelectSubtreeAction = new QAction(trUtf8("Select subtree"), this);
	connect(m_oSelectSubtreeAction, SIGNAL(triggered()), this, SLOT(slot_select_subtree()));
	addAction(m_oSelectSubtreeAction);


	m_oNextRootAction = l_o = new QAction(trUtf8("Next root"), this); l_o->setShortcut(notr("PgDown")); connect(l_o, SIGNAL(triggered()), this, SLOT(slot_next_root())); addAction(l_o); l_o->setData(QVariant(1));


	m_oEditAction = new QAction(trUtf8("Toggle edit"), this);
	m_oEditAction->setShortcut(notr("Return"));
	addAction(m_oEditAction);
	connect(m_oEditAction, SIGNAL(triggered()), this, SLOT(slot_toggle_edit()));

	m_oCancelEditAction = new QAction(trUtf8("Cancel edit"), this);
	m_oCancelEditAction->setShortcut(notr("Escape"));
	addAction(m_oCancelEditAction);
	connect(m_oCancelEditAction, SIGNAL(triggered()), this, SLOT(slot_cancel_edit()));
	m_oCancelEditAction->setEnabled(false);

	m_oMenu = new QMenu(this);
	m_oMenu->addAction(m_oAddItemAction);
	//m_oMenu->addAction(m_oEditAction);
	m_oMenu->addAction(m_oDeleteAction);
	m_oMenu->addAction(m_oSelectSubtreeAction);

	QAction *fullAction = new QAction(trUtf8("Toggle fullscreen"), this);
	connect(fullAction, SIGNAL(triggered()), this, SLOT(toggle_fullscreen())); addAction(l_o);
	m_oMenu->addAction(fullAction);

	m_oColorMenu = m_oMenu->addMenu(trUtf8("Colors"));
	m_oDataMenu = m_oMenu->addMenu(trUtf8("Data type"));


#define newAction(s, v, dest)  dest = l_o = new QAction(s, this); \
	connect(l_o, SIGNAL(triggered()), this, SLOT(slot_change_data())); \
	addAction(l_o); l_o->setData(QVariant(v)); \
	m_oDataMenu->addAction(l_o); \
	l_o->setCheckable(true);

	newAction(trUtf8("Text"), VIEW_TEXT, m_oTextType);
	newAction(trUtf8("Diagram"), VIEW_DIAG, m_oDiagramType);
	newAction(trUtf8("Table"), VIEW_TABLE, m_oTableType);
	newAction(trUtf8("Image"), VIEW_IMG, m_oImageType);

	m_bDeleting = false;
	//connect(scene(), SIGNAL(selectionChanged()), this, SLOT(selection_changed())); // TODO check with m_bPressed

	setDragMode(QGraphicsView::RubberBandDrag);
}

void canvas_view::check_selection() {

	if (m_bDeleting) return;

	QList<int> lst;
	foreach(canvas_item* k, selection()) {
		lst.append(k->Id());
	}

	// stupid set intersection
	QList<int> unlst;
	foreach(data_item* x, m_oMediator->m_oItems.values()) {
		if (x->m_bSelected) {
			unlst.append(x->m_iId);
		}
	}

	foreach(int i, lst) {
		if (!unlst.contains(i))
			goto undo;
	}

	foreach (int i, unlst) {
		if (!lst.contains(i))
			goto undo;
	}

	mem_sel *sel;
	goto end;
	undo:
		sel = new mem_sel(m_oMediator);
		sel->sel = lst;
		sel->apply();
		//qDebug()<<"selected"<<lst<<"unselected"<<unlst;
	end:
		;
}

void canvas_view::slot_next_root()
{
	switch (((QAction*) QObject::sender())->data().toInt())
	{
		case 0:
			m_oMediator->prev_root();
			break;
		case 1:
			m_oMediator->next_root();
			break;
		default:
			break;
	}
	QList<canvas_item*> sel = selection();
	if (sel.size()==1) ensureVisible(sel[0]);
}

void canvas_view::edit_off() {
	canvas_item* sel = NULL;
	foreach (QGraphicsItem *tmp, items()) {
		if (tmp->type() == CANVAS_ITEM_T) {
			sel = (canvas_item*) tmp;

			if (sel->textInteractionFlags() & Qt::TextEditorInteraction) {

				sel->setTextInteractionFlags(Qt::NoTextInteraction);
				if (sel->toPlainText() == QObject::notr("")) {
					sel->setPlainText(QObject::trUtf8("Empty"));
					sel->update_links();
				}

				if (sel->toPlainText() != (*m_oMediator + sel->Id())->m_sSummary) {
					mem_edit *ed = new mem_edit(m_oMediator);
					ed->newSummary = sel->toPlainText();
					ed->apply();
				}

				m_oAddItemAction->setEnabled(true);
				m_oInsertSiblingAction->setEnabled(true);
				m_oDeleteAction->setEnabled(true);
				m_oNextRootAction->setEnabled(true);

				m_oMoveUpAction->setEnabled(true);
				m_oMoveDownAction->setEnabled(true);
				m_oMoveLeftAction->setEnabled(true);
				m_oMoveRightAction->setEnabled(true);
				m_oSelectUpAction->setEnabled(true);
				m_oSelectDownAction->setEnabled(true);
				m_oSelectLeftAction->setEnabled(true);
				m_oSelectRightAction->setEnabled(true);
				m_oSelectSubtreeAction->setEnabled(true);
				m_oMediator->check_undo(true);
			}
		}
	}
}

void canvas_view::slot_toggle_edit()
{
	if (!hasFocus()) {
		return;
	}

	canvas_item* sel = NULL;
	foreach (QGraphicsItem *tmp, items()) {
		if (tmp->type() == CANVAS_ITEM_T && tmp->isSelected()) {
			if (sel) {
				sel = NULL;
				break;
			} else {
				sel = (canvas_item*) tmp;
			}
		}
	}

	if (sel) {
		if (sel->textInteractionFlags() & Qt::TextEditorInteraction) {
			sel->setTextInteractionFlags(Qt::NoTextInteraction);
			if (sel->toPlainText() == QObject::notr("")) {
				sel->setPlainText(QObject::trUtf8("Empty"));
				sel->update(); // seems to update faster
				sel->update_links();

			}
			if (sel->toPlainText() != (*m_oMediator + sel->Id())->m_sSummary) {
				mem_edit *ed = new mem_edit(m_oMediator);
				ed->newSummary = sel->toPlainText();
				ed->apply();
			}
		} else {
			sel->setTextInteractionFlags(Qt::TextEditorInteraction);
			if (sel->toPlainText() == QObject::trUtf8("Empty")) {
				sel->setPlainText("");
				sel->update(); // seems to update faster
				sel->update_links();
			}
			sel->setFocus();

			m_oAddItemAction->setEnabled(false);
			m_oCancelEditAction->setEnabled(true);
			m_oInsertSiblingAction->setEnabled(false);
			m_oDeleteAction->setEnabled(false);
			m_oNextRootAction->setEnabled(false);

			m_oMoveUpAction->setEnabled(false);
			m_oMoveDownAction->setEnabled(false);
			m_oMoveLeftAction->setEnabled(false);
			m_oMoveRightAction->setEnabled(false);
			m_oSelectUpAction->setEnabled(false);
			m_oSelectDownAction->setEnabled(false);
			m_oSelectLeftAction->setEnabled(false);
			m_oSelectRightAction->setEnabled(false);
			m_oSelectSubtreeAction->setEnabled(false);
			m_oMediator->check_undo(false);
			return;
		}
	}

	m_oAddItemAction->setEnabled(true);
	m_oCancelEditAction->setEnabled(false);
	m_oInsertSiblingAction->setEnabled(true);
	m_oDeleteAction->setEnabled(true);
	m_oNextRootAction->setEnabled(true);

	m_oMoveUpAction->setEnabled(true);
	m_oMoveDownAction->setEnabled(true);
	m_oMoveLeftAction->setEnabled(true);
	m_oMoveRightAction->setEnabled(true);
	m_oSelectUpAction->setEnabled(true);
	m_oSelectDownAction->setEnabled(true);
	m_oSelectLeftAction->setEnabled(true);
	m_oSelectRightAction->setEnabled(true);
	m_oSelectSubtreeAction->setEnabled(true);
	m_oMediator->check_undo(true);
}

void canvas_view::slot_cancel_edit()
{
	canvas_item* sel = NULL;
	foreach (QGraphicsItem *tmp, items()) {
		if (tmp->type() == CANVAS_ITEM_T && tmp->isSelected()) {
			if (sel) {
				sel = NULL;
				break;
			} else {
				sel = (canvas_item*) tmp;
			}
		}
	}

	if (sel && sel->textInteractionFlags() & Qt::TextEditorInteraction) {
		sel->setTextInteractionFlags(Qt::NoTextInteraction);
		sel->update_data();
	}

	m_oAddItemAction->setEnabled(true);
	m_oCancelEditAction->setEnabled(false);
	m_oInsertSiblingAction->setEnabled(true);
	m_oDeleteAction->setEnabled(true);
	m_oNextRootAction->setEnabled(true);

	m_oMoveUpAction->setEnabled(true);
	m_oMoveDownAction->setEnabled(true);
	m_oMoveLeftAction->setEnabled(true);
	m_oMoveRightAction->setEnabled(true);
	m_oSelectUpAction->setEnabled(true);
	m_oSelectDownAction->setEnabled(true);
	m_oSelectLeftAction->setEnabled(true);
	m_oSelectRightAction->setEnabled(true);
	m_oSelectSubtreeAction->setEnabled(true);
	m_oMediator->check_undo(true);
}

void canvas_view::slot_move()
{
	QList<canvas_item*> sel = selection();
	if (sel.size() < 1) return;
	switch (((QAction*) QObject::sender())->data().toInt())
	{
		case 0: move_sel(0, -20); break;
		case 1: move_sel(0, 20); break;
		case 2: move_sel(-20, 0); break;
		case 3: move_sel(20, 0); break;
		default: break;
	}
}

void canvas_view::slot_sel()
{
	QList<canvas_item*> sel = selection();
	if (sel.size() != 1) return;
	int l_iId = sel[0]->Id();
	switch (((QAction*) QObject::sender())->data().toInt())
	{
		case 0: m_oMediator->select_item_keyboard(l_iId, 3); break;
		case 1: m_oMediator->select_item_keyboard(l_iId, 4); break;
		case 2: m_oMediator->select_item_keyboard(l_iId, 1); break;
		case 3: m_oMediator->select_item_keyboard(l_iId, 2); break;
		default: break;
	}
	if (sel.size()==1) ensureVisible(sel[0]);
}

void canvas_view::slot_add_item()
{
	QList<canvas_item*> sel = selection();
	int l_iId = NO_ITEM;
	if (sel.size() == 1) {
		l_iId = sel[0]->Id();
	}

	deselect_all();

	mem_add *add = new mem_add(m_oMediator);
	add->init();
	add->item->m_iXX = m_oLastPoint.x();
	add->item->m_iYY = m_oLastPoint.y();
	add->parent = l_iId;
	add->apply();

	reorganize(); // this was here before
}

void canvas_view::slot_add_sibling()
{
	QList<canvas_item*> sel = selection();
	if (sel.size() != 1) return;
	int l_iId = m_oMediator->parent_of(sel[0]->Id());
	if (l_iId == NO_ITEM) return;
	deselect_all();

	mem_add *add = new mem_add(m_oMediator);
	add->init();
	add->item->m_iXX = m_oLastPoint.x();
	add->item->m_iYY = m_oLastPoint.y();
	add->parent = l_iId;
	add->apply();
	reorganize(); // this was here before
}

void canvas_view::slot_delete()
{
	QList<int> l_oLst;
	foreach (canvas_item *l_oItem, selection()) {
		l_oLst.push_back(l_oItem->Id());
	}

	if (l_oLst.isEmpty()) {
		return;
	}

	mem_sel *sel = new mem_sel(m_oMediator);
	sel->unsel = l_oLst;
	sel->apply();

	mem_delete *del = new mem_delete(m_oMediator);
	del->init(l_oLst);
	del->apply();
}

void canvas_view::show_sort(int i_iId, bool i_b)
{
	int j=0;
	for (int i=0; i<m_oMediator->m_oLinks.size(); i++)
	{
		QPoint l_oP = m_oMediator->m_oLinks.at(i);
		if (l_oP.x() == i_iId)
		{
			++j;
			canvas_item *l_oRect = m_oItems.value(l_oP.y());

			if (j != l_oRect->m_iNum)
			{
				l_oRect->m_iNum = j;
			}
			l_oRect->m_oSort->update();
			l_oRect->m_oSort->setVisible(i_b);
		}
	}
}

void canvas_view::move_sel(int i_iX, int i_iY)
{
	QList<canvas_item*> sel = selection();
	check_canvas_size();
	foreach (canvas_item *l_oItem, sel) { l_oItem->moveBy(i_iX, i_iY); }
	foreach (canvas_item *l_oItem, sel) { l_oItem->update_links(); ensureVisible(l_oItem); }
}

void canvas_view::zoom_in()
{
	double i_iScaleFactor = 1.05;
	qreal i_rFactor = matrix().scale(i_iScaleFactor, i_iScaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
	if (i_rFactor < 0.01 || i_rFactor > 1000) return;
	scale(i_iScaleFactor, i_iScaleFactor);
	check_canvas_size();
}

void canvas_view::zoom_out()
{
	double i_iScaleFactor = 0.95;
	qreal i_rFactor = matrix().scale(i_iScaleFactor, i_iScaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
	if (i_rFactor < 0.01 || i_rFactor > 1000) return;
	scale(i_iScaleFactor, i_iScaleFactor);
	check_canvas_size();
}

void canvas_view::wheelEvent(QWheelEvent *i_oEvent)
{
	QPointF l_o = mapToScene(i_oEvent->pos());
	qreal i_iScaleFactor = pow(2.0, i_oEvent->delta() / 440.0);
	qreal i_rFactor = matrix().scale(i_iScaleFactor, i_iScaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
	if (i_rFactor < 0.01 || i_rFactor > 1000) return;
	scale(i_iScaleFactor, i_iScaleFactor);
	centerOn(l_o + mapToScene(viewport()->rect().center()) - mapToScene(i_oEvent->pos()));
}

void canvas_view::notify_open_map() {
	QRect l_oRect = viewport()->rect();
	if (m_oItems.size() < 1) // no rectangle
	{
		scene()->setSceneRect(QRectF(mapToScene(l_oRect.topLeft()), mapToScene(l_oRect.bottomRight())));
		return;
	}

	check_canvas_size();
	fitInView(scene()->sceneRect(), Qt::KeepAspectRatio);
}

void canvas_view::notify_select(const QList<int>& unsel, const QList<int>& sel) {

	edit_off();
	foreach (int k, sel) {
		Q_ASSERT(m_oItems[k] != NULL);
		if (!m_oItems[k]->isSelected())
		{
			m_oItems[k]->setSelected(true);
		}
	}

	foreach (int k, unsel) {
		Q_ASSERT(m_oItems[k] != NULL);
		if (m_oItems[k]->isSelected())
		{
			m_oItems[k]->setSelected(false);
		}
	}

	if (m_iSortId != NO_ITEM)
	{
		show_sort(m_iSortId, false);
		m_iSortCursor = 0;
		m_iSortId = NO_ITEM;
	}

	if (sel.size() == 1 && true) //
	{
		m_iSortId = sel.at(0);
		m_iSortCursor = 0;
	}
}

void canvas_view::sync_colors() {
	semantik_win *l_o = (semantik_win*) m_oSemantikWindow;
	l_o->m_oColorsToolBar->clear();

	while (l_o->m_oColorGroup->actions().size() > m_oMediator->m_oColorSchemes.size()+1)
	{
		QAction* l_oA = l_o->m_oColorGroup->actions().takeFirst();
		l_o->m_oColorsToolBar->removeAction(l_oA);
		m_oColorMenu->removeAction(l_oA);
		delete l_oA;
	}

	while (l_o->m_oColorGroup->actions().size() < m_oMediator->m_oColorSchemes.size()+1)
	{
		new QAction(QIcon(), trUtf8("Color"), l_o->m_oColorGroup);
	}

	l_o->m_oColorGroup->removeAction(l_o->m_oCustomColorAct);
	l_o->m_oColorGroup->addAction(l_o->m_oCustomColorAct);
	m_oColorMenu->removeAction(l_o->m_oCustomColorAct);
	m_oColorMenu->addAction(l_o->m_oCustomColorAct);

	for (int i=0; i<m_oMediator->m_oColorSchemes.size(); ++i)
	{
		color_scheme l_oScheme = m_oMediator->m_oColorSchemes[i];
		QAction *l_oAction = l_o->m_oColorGroup->actions()[i];

		QPixmap l_oPix(22, 22);
		QPainter l_oP(&l_oPix);

		l_oAction->setText(l_oScheme.m_sName);

		l_oPix.fill(l_oScheme.m_oInnerColor);
		//TODO pen ?
		l_oP.drawRect(0, 0, 21, 21);
		l_oAction->setIcon(QIcon(l_oPix));
	}

	// la première action est pour la couleur de la racine
	for (int i=1; i<l_o->m_oColorGroup->actions().size(); ++i)
	{
		QAction *l_oAct = l_o->m_oColorGroup->actions()[i];
		l_o->m_oColorsToolBar->addAction(l_oAct);
		m_oColorMenu->addAction(l_oAct);
	}
}

void canvas_view::notify_pic(int id)
{
	qDebug()<<"canvas_view::notify_pic to be implemented";
}

void canvas_view::sync_flags() {
	semantik_win *l_o = (semantik_win*) m_oSemantikWindow;
	l_o->m_oFlagsToolBar->clear();

	while (l_o->m_oFlagGroup->actions().size() > m_oMediator->m_oFlagSchemes.size())
	{
		QAction* l_oA = l_o->m_oFlagGroup->actions().takeFirst();
		delete l_oA;
	}

	while (l_o->m_oFlagGroup->actions().size() < m_oMediator->m_oFlagSchemes.size())
	{
		new QAction(QIcon(), trUtf8("Flag"), l_o->m_oFlagGroup);
	}

	for (int i=0; i<m_oMediator->m_oFlagSchemes.size(); ++i)
	{
		flag_scheme* l_oScheme = m_oMediator->m_oFlagSchemes[i];
		QAction *l_oAction = l_o->m_oFlagGroup->actions()[i];

		l_oAction->setText(l_oScheme->m_sName);
		l_oAction->setIcon(l_oScheme->_icon());
	}

	foreach(QAction* l_oAct, l_o->m_oFlagGroup->actions())
	{
		l_o->m_oFlagsToolBar->addAction(l_oAct);
		l_oAct->setCheckable(true);
	}
}

void canvas_view::change_colors(QAction* i_oAct)
{
	if (! hasFocus()) return;

	static QColor l_oColor;

	int l_iIdx = -1;
	semantik_win *l_o = (semantik_win*) m_oSemantikWindow;
	for (int i=1; i<l_o->m_oColorGroup->actions().size(); ++i)
	{
		if (l_o->m_oColorGroup->actions()[i]==i_oAct)
		{
			l_iIdx = i;
			if (i==l_o->m_oColorGroup->actions().size()-1)
			{
				l_oColor = QColorDialog::getColor(l_oColor, this);
				if (!l_oColor.isValid()) return;
			}
			break;
		}
	}

	mem_color* col = new mem_color(m_oMediator);
	col->newColor = l_iIdx;
	col->apply();
}

void canvas_view::change_flags(QAction* i_oAction)
{
	int l_iIndex = 0;
	semantik_win *l_o = (semantik_win*) m_oSemantikWindow;
	for (int i=0; i<l_o->m_oFlagGroup->actions().size(); ++i)
	{
		QAction *l_oAct = l_o->m_oFlagGroup->actions()[i];
		if (l_oAct == i_oAction) l_iIndex = i;
	}
	QString l_sName = m_oMediator->m_oFlagSchemes[l_iIndex]->m_sId;

	/*
	canvas_item *l_oItem = selection()[0];
	data_item *l_oData = *m_oMediator + l_oItem->Id();

	if (i_oAction->isChecked()) l_oData->m_oFlags.push_back(l_sName);
	else l_oData->m_oFlags.removeAll(l_sName);
	l_oItem->update_flags(); */

	mem_flag* tmp = new mem_flag(m_oMediator);
	tmp->flag = l_sName;
	tmp->add = i_oAction->isChecked();
	tmp->apply();
}

void canvas_view::check_selected()
{
	QList <canvas_item*> sel = selection();
	semantik_win *l_o = (semantik_win*) m_oSemantikWindow;
	data_item *l_oData = NULL;
	if (sel.size() == 1)
	{
		l_oData = *m_oMediator + sel[0]->Id();
	}

	//foreach(QAction* l_oAct, l_o->m_oFlagGroup->actions())
	for (int i=0; i<l_o->m_oFlagGroup->actions().size(); ++i)
	{
		QAction *l_oAct = l_o->m_oFlagGroup->actions()[i];

		if (sel.size() != 1) l_oAct->setChecked(false);
		else
		{
			QString l_sName = m_oMediator->m_oFlagSchemes[i]->m_sId;
			l_oAct->setChecked(l_oData->m_oFlags.contains(l_sName));
		}
		l_oAct->setEnabled(sel.size());
	}
	foreach(QAction* l_oAct, l_o->m_oColorGroup->actions())
	{
		l_oAct->setEnabled(sel.size() >= 1);
	}
}

void canvas_view::deselect_all(bool i_oSignal)
{
	edit_off();
	mem_sel *sel = new mem_sel(m_oMediator);
	if (sel->unsel.empty())
	{
		delete sel;
	}
	else
	{
		sel->apply();
	}
}

void canvas_view::enable_actions()
{
	foreach (QAction* l_o, actions())
	{
		l_o->setEnabled(true); // TODO we could disable the move actions one by one
	}
	enable_menu_actions();
}

void canvas_view::enable_menu_actions()
{
	QList<canvas_item*> sel = selection();
	m_oAddItemAction->setEnabled(sel.size()<=1);
	m_oDeleteAction->setEnabled(sel.size()>0);
	//m_oEditAction->setEnabled(sel.size()==1);
	m_oColorMenu->setEnabled(sel.size()>=1);
	m_oDataMenu->setEnabled(sel.size()==1);
	m_oSelectSubtreeAction->setEnabled(sel.size()==1);

	if (sel.size() == 1)
	{
		data_item *l_oData = *m_oMediator + sel[0]->Id();

		#define fafa(v, t) v->setChecked(l_oData->m_iDataType == t);
		fafa(m_oTextType, VIEW_TEXT);
		fafa(m_oDiagramType, VIEW_DIAG);
		fafa(m_oTableType, VIEW_TABLE);
		fafa(m_oImageType, VIEW_IMG);
	}

	foreach (QAction* l_o, m_oDataMenu->actions())
	{
		l_o->setEnabled(sel.size()==1);
	}
}

void canvas_view::mousePressEvent(QMouseEvent *i_oEv)
{
	if (i_oEv->button() == Qt::MidButton) {
		m_bScroll = true;
		setDragMode(QGraphicsView::ScrollHandDrag);
		return;
	}

	m_oLastPressPoint = i_oEv->pos();
	if (i_oEv->button() == Qt::RightButton)
	{
		// first, we cannot edit an item when right-click is selected
		edit_off();

		// select the item under the cursor if available and show the popup menu
		m_oLastPoint = mapToScene(i_oEv->pos());
		QGraphicsItem *l_oItem = scene()->itemAt(mapToScene(i_oEv->pos()));
		if (l_oItem && l_oItem->type() == CANVAS_ITEM_T)
		{
			if (!l_oItem->isSelected())
			{
				QList<int> lst;
				lst.append(((canvas_item*) l_oItem)->Id());

				mem_sel *sel = new mem_sel(m_oMediator);
				sel->sel = lst;
				sel->apply();
			}
		}
		else
		{
			deselect_all();
		}
		enable_menu_actions();
		m_oMenu->popup(i_oEv->globalPos());
		i_oEv->accept();
		return;
	}

	QGraphicsItem *l_oItem = scene()->itemAt(mapToScene(i_oEv->pos()));
	QList<canvas_item*> sel = selection();
	if (sel.size() == 1 && QApplication::keyboardModifiers() & Qt::ShiftModifier)
	{
		// link items on click sequences
		if (l_oItem && l_oItem->type() == CANVAS_ITEM_T) {

			int id1 = sel.at(0)->Id();
			int id2 = ((canvas_item*) l_oItem)->Id();
			m_oMediator->link_items(id1, id2);

			QList<int> unlst;
			unlst.append(id1);
			QList<int> lst;
			lst.append(id2);

			mem_sel *sel = new mem_sel(m_oMediator);
			sel->sel = lst;
			sel->unsel = unlst;
			sel->apply();
			return;
		}
	}

	canvas_chain *kk=NULL;
	if (l_oItem && (kk = dynamic_cast<canvas_chain*>(l_oItem)))
	{
		QList<int> lst;
		foreach (QGraphicsItem *l_o, scene()->selectedItems())
		{
			l_o->setSelected(false);
			canvas_item *it = dynamic_cast<canvas_item*>(l_o);
			if (it)
			{
				lst.append(it->Id());
			}
		}

		if (lst.size())
		{
			mem_sel *sel = new mem_sel(m_oMediator);
			sel->unsel = lst;
			sel->apply();
		}

		canvas_item *l_oParent = dynamic_cast<canvas_item*>(kk->parentItem());
		Q_ASSERT(l_oParent);

		QRectF r = l_oParent->boundingRect().translated(l_oParent->pos());
		m_oLastPressPoint.setX(r.x() + r.width() / 2.);
		m_oLastPressPoint.setY(r.y() + r.height() / 2.);
		m_oLastPressPoint = mapFromScene(m_oLastPressPoint);
		
		QRect l_oSel = QRect(m_oLastPressPoint, i_oEv->pos());
		m_oRubberLine->setGeometry(l_oSel);

		m_oRubberLine->setVisible(true);
		return;
	}

        canvas_sort *l_oSort = dynamic_cast<canvas_sort*>(scene()->itemAt(mapToScene(i_oEv->pos())));
	if (l_oSort)
		return;
	canvas_sort_toggle *l_oToggle = dynamic_cast<canvas_sort_toggle*>(l_oItem);
	if (l_oToggle)
		return;
	QGraphicsView::mousePressEvent(i_oEv);
}


void canvas_view::mouseReleaseEvent(QMouseEvent *i_oEv)
{
	if (m_oRubberLine->isVisible())
	{
		m_oRubberLine->setVisible(false);
		canvas_item *l_oR1 = NULL;
		canvas_item *l_oR2 = NULL;

		foreach (QGraphicsItem *l_oI1, scene()->items(mapToScene(m_oLastPressPoint)))
		{
			if (l_oI1->type() == CANVAS_ITEM_T)
			{
				l_oR1 = (canvas_item*) l_oI1;
				break;
			}
		}

		foreach (QGraphicsItem *l_oI1, scene()->items(mapToScene(i_oEv->pos())))
		{
			if (l_oI1->type() == CANVAS_ITEM_T)
			{
				l_oR2 = (canvas_item*) l_oI1;
				break;
			}
		}

		if (l_oR1 && l_oR2 && l_oR1 != l_oR2)
		{
			m_oMediator->link_items(l_oR1->Id(), l_oR2->Id());
			deselect_all(); // TODO
		}
		m_oRubberLine->hide();
		return;
	}

	if (m_bScroll)
	{
		m_bScroll = false;
		setDragMode(QGraphicsView::RubberBandDrag);
	}

	QGraphicsItem *l_oItem = scene()->itemAt(mapToScene(i_oEv->pos()));
	canvas_sort *l_oSort = dynamic_cast<canvas_sort*>(l_oItem);
	if (l_oSort)
	{
		int l_iId = l_oSort->m_oFrom->Id();
		int l_iParentId = m_oMediator->parent_of(l_iId);

		mem_sort *srt = new mem_sort(m_oMediator);
		srt->init(l_iParentId, l_iId, m_iSortCursor);
		srt->apply();

		m_iSortCursor++;
		if (m_iSortCursor >= m_oMediator->num_children(l_iParentId))
		{
			m_iSortCursor = 0;
		}

		emit sig_message(trUtf8("Click to set Item %1").arg(QString::number(m_iSortCursor+1)), -1);
		return;
	}

	canvas_sort_toggle *l_oToggle = dynamic_cast<canvas_sort_toggle*>(l_oItem);
	if (l_oToggle)
	{
		//check_selection(); // TODO ITA
		if (scene()->selectedItems().size() == 1)
		{
			canvas_item *it = static_cast<canvas_item*>(l_oToggle->parentItem());
			m_iSortId = it->Id();
			m_iSortCursor = 0;
			show_sort(it->Id(), true);
		}
		return;
	}
	QGraphicsView::mouseReleaseEvent(i_oEv);

	edit_off();

	if (i_oEv->button() == Qt::RightButton) return;
	m_bPressed = false;

	check_selection();

	//qDebug()<<"mouse release event!";
	QList<int> lst;
	foreach (QGraphicsItem* k, scene()->selectedItems()) {
		canvas_item* t = (canvas_item*) k;
		lst.append(t->Id());
	}
	if (lst.size()) {
		data_item *p = m_oMediator->m_oItems[lst[0]];
		canvas_item *v = m_oItems[lst[0]];
		if (qAbs(p->m_iXX - v->pos().x()) + qAbs(p->m_iYY - v->pos().y()) > 0.1) {
			mem_move *mv = new mem_move(m_oMediator);
			mv->sel = lst;
			for (int i = 0; i < lst.size(); ++i) {
				data_item *it = m_oMediator->m_oItems[lst[i]];
				mv->oldPos.append(QPointF(it->m_iXX, it->m_iYY));
				mv->newPos.append(m_oItems[lst[i]]->pos());
			}
			mv->apply();
		} // else { qDebug()<<"move too small"; }
	}

	check_canvas_size();
}

void canvas_view::mouseDoubleClickEvent(QMouseEvent* i_oEv)
{
	if (i_oEv->button() != Qt::LeftButton) return;
	m_oLastPoint = mapToScene(i_oEv->pos());
	QGraphicsItem *l_oItem = itemAt(i_oEv->pos());

	if (l_oItem) {
		if (l_oItem->type() == CANVAS_ITEM_T) {
			deselect_all();

			canvas_item *l_oR = (canvas_item*) l_oItem;
			l_oR->setSelected(false);

			mem_add *add = new mem_add(m_oMediator);
			add->init();
			add->item->m_iXX = m_oLastPoint.x();
			add->item->m_iYY = m_oLastPoint.y();
			add->parent = l_oR->Id();
			add->apply();
		} else if (l_oItem->type() == CANVAS_LINK_T) {
			canvas_link *l_oLink = (canvas_link*) l_oItem;
			mem_unlink *link = new mem_unlink(m_oMediator);
			link->child = l_oLink->m_oTo->Id();
			link->parent = l_oLink->m_oFrom->Id();
			link->apply();
		}
	} else if (i_oEv->modifiers() != Qt::ControlModifier) {
		mem_add *add = new mem_add(m_oMediator);
		add->init();
		add->item->m_iXX = m_oLastPoint.x();
		add->item->m_iYY = m_oLastPoint.y();
		add->apply();
	}
}

void canvas_view::check_canvas_size()
{
	QRectF br = scene()->itemsBoundingRect();
	br.adjust(-GAP, -GAP, GAP, GAP);
	br = br.united(scene()->sceneRect());
	scene()->setSceneRect(br);
}

void canvas_view::fit_zoom()
{
	//QRectF l_o = scene()->sceneRect();
	//fitInView(QRectF(l_o.topLeft()+QPointF(100, 100), l_o.size()+QSizeF(200, -200)), Qt::KeepAspectRatio);
#if 0
	QRectF l_o;
	foreach (QGraphicsItem *it, items())
	{
		l_o |= it->boundingRect();
	}
#endif
	//check_canvas_size();
	fitInView(scene()->sceneRect(), Qt::KeepAspectRatio);
}

void canvas_view::slot_change_data()
{
	mem_datatype* t = new mem_datatype(m_oMediator);
	t->newDataType = ((QAction*) QObject::sender())->data().toInt();
	if (t->sel != NULL && t->newDataType != t->oldDataType) t->apply();
	else delete(t);
}

void canvas_view::toggle_fullscreen()
{
	if (isFullScreen())
	{
		setWindowModality(Qt::NonModal);
		//setFullScreen(false);
		setWindowState(Qt::WindowNoState);
		setWindowFlags(Qt::Widget);
		show();
	}
	else
	{
		setWindowFlags(Qt::Window);
		//setFullScreen(true);
		setWindowState(Qt::WindowFullScreen);
		setWindowModality(Qt::ApplicationModal);
		show();
	}
}

rubber_line::rubber_line(QRubberBand::Shape i, QWidget* j) : QRubberBand(i, j)
{
}

void rubber_line::paintEvent(QPaintEvent *)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);
	//painter.setPen(Qt::NoPen);
	painter.setBrush(QColor(Qt::red));
	if (_direction > 0)
	{
		painter.drawLine(QPoint(0, 0), QPoint(size().width(), size().height()));
	}
	else
	{
		painter.drawLine(QPoint(size().width(), 0), QPoint(0, size().height()));
	}
}

void rubber_line::setGeometry(const QRect& i_o)
{
	_direction = i_o.width() * i_o.height();
	QRubberBand::setGeometry(i_o.normalized());
}

#define HSPACER 32.
#define WSPACER 32.

double canvas_view::compute_height(QMap<int, double> &map, QMap<int, QList<int> >&children, int id) {
	double size = 0;

	QMap<int, QList<int> >::const_iterator it = children.find(id);
	if (it != children.end()) {
		QList<int> tmp = it.value();
		size += (tmp.size() - 1) * HSPACER;
		foreach (int k, tmp) {
			size += compute_height(map, children, k);
		}
	}

	double tmp = m_oItems[id]->boundingRect().height();
	if (size < tmp) size = tmp;

	map[id] = size;
	//qDebug()<<"size for"<<id<<" "<<size;
	return size;
}

void canvas_view::compute_width(QMap<int, double> &map, QMap<int, QList<int> >&children, int id, int level) {
	double w = m_oItems[id]->boundingRect().width();
	QMap<int, double>::iterator jt = map.find(level);
	if (jt != map.end()) {
		double val = jt.value();
		map[level] = val > w ? val : w;
	} else {
		map[level] = w;
	}

	QMap<int, QList<int> >::iterator it = children.find(id);
	if (it != children.end()) {
		QList<int> tmp = it.value();
		foreach (int sub, tmp) {
			compute_width(map, children, sub, level+1);
		}
	}
}

void canvas_view::reorganize() {
	QList<int> roots = m_oMediator->all_roots();
	QMap<int, double> height;

	QMap<int, QList<int> > children;

	for (int i=0; i < m_oMediator->m_oLinks.size(); ++i) {
		QPoint l_oP = m_oMediator->m_oLinks.at(i);

		QMap<int, QList<int> >::iterator it = children.find(l_oP.x());
		if (it != children.end()) {
			it.value() << l_oP.y();
		} else {
			QList<int> tmp;
			tmp << l_oP.y();
			children[l_oP.x()] = tmp;
		}
	}

	foreach (int k, roots) {
		double ref = compute_height(height, children, k);
		QMap<int, QList<int> >::iterator it = children.find(k);
		if (it != children.end()) {
			QList<int> tmp = it.value();

			ref -= (tmp.size() - 1) * HSPACER;
			int mid = 0;
			int max = 0;
			int tot = 0;
			double left_height = 0;
			foreach (int sub, tmp) {
				tot += height[sub];
				if (tot * (ref - tot) >= max) {
					max = tot * (ref - tot);
					mid = sub;

					left_height += height[sub] + HSPACER;
				} else {
					break;
				}
			}

			QMap<int, double> width;
			compute_width(width, children, k, 0);

			left_height -= HSPACER;

			int left = 1;
			double acc_height = m_oItems[k]->y() + m_oItems[k]->boundingRect().height() / 2 - left_height / 2;
			foreach (int sub, tmp) {

				// put the element in place, then recurse

				double y = acc_height + height[sub] / 2 - m_oItems[sub]->boundingRect().height() / 2;
				if (left) {
					double x = m_oItems[k]->x() + m_oItems[k]->boundingRect().width() - width[0] - WSPACER;
					m_oItems[sub]->setPos(x - m_oItems[sub]->boundingRect().width(), y);
				} else {
					double x = m_oItems[k]->x() + width[0] + WSPACER;
					m_oItems[sub]->setPos(x, y);
				}

				acc_height += height[sub] + HSPACER;

				m_oItems[k]->update_links();
				pack(width, height, children, sub, 1, left);

				// now to the right
				if (sub == mid) {
					left = 0;
					acc_height = m_oItems[k]->y() + m_oItems[k]->boundingRect().height() / 2 - (height[k] - left_height - HSPACER) / 2;
				}
			}
		}
	}
}

void canvas_view::pack(QMap<int, double> &width, QMap<int, double> &height, QMap<int, QList<int> >&children, int id, int level, int left) {
	QMap<int, QList<int> >::iterator it = children.find(id);
	if (it != children.end()) {
		QList<int> tmp = it.value();
		double acc_height = m_oItems[id]->y() + m_oItems[id]->boundingRect().height() / 2 - height[id] / 2;
		foreach (int sub, tmp) {
			double y = acc_height + height[sub] / 2 - m_oItems[sub]->boundingRect().height()/2;
			if (left) {
				double x = m_oItems[id]->x() + m_oItems[id]->boundingRect().width() - width[0] - WSPACER;
				m_oItems[sub]->setPos(x - m_oItems[sub]->boundingRect().width(), y);
			} else {
				double x = m_oItems[id]->x() + width[0] + WSPACER;
				m_oItems[sub]->setPos(x, y);
			}
			acc_height += height[sub] + HSPACER;
			pack(width, height, children, sub, level+1, left);
		}
		m_oItems[id]->update_links();
	}
}

void canvas_view::export_map_size()
{
	QRectF l_oRect = scene()->itemsBoundingRect();
        l_oRect = QRectF(l_oRect.topLeft() - QPointF(25, 25), l_oRect.bottomRight() + QPointF(25, 25));

	export_map_dialog* exp = new export_map_dialog(this);

	exp->kurlrequester->setMode(KFile::File | KFile::LocalOnly);
	exp->kurlrequester->setFilter(trUtf8("*.png|PNG Files (*.png)"));

	exp->kurlrequester->setUrl(KUrl(m_oMediator->m_sExportUrl));
	exp->m_oWidthC->setChecked(m_oMediator->m_bExportIsWidth);
	exp->m_iBaseWidth = l_oRect.width();
	exp->m_iBaseHeight = l_oRect.height();

	if (m_oMediator->m_bExportIsWidth)
	{
		if (m_oMediator->m_iExportWidth > 0)
			exp->m_oWidth->setValue(m_oMediator->m_iExportWidth);
		else
			exp->m_oWidth->setValue(l_oRect.width());
	}
	else
	{
		if (m_oMediator->m_iExportHeight > 0)
			exp->m_oHeight->setValue(m_oMediator->m_iExportHeight);
		else
			exp->m_oHeight->setValue(l_oRect.height());
	}

	if (exp->exec() == KDialog::Accepted)
	{
		if (m_oMediator->m_iExportWidth != exp->m_oWidth->value())
		{
			m_oMediator->m_iExportWidth = exp->m_oWidth->value();
			m_oMediator->set_dirty();
		}

		if (m_oMediator->m_iExportHeight != exp->m_oHeight->value())
		{
			m_oMediator->m_iExportHeight = exp->m_oHeight->value();
			m_oMediator->set_dirty();
		}

		if (!exp->kurlrequester->url().isValid() || exp->kurlrequester->url().isEmpty())
		{
			m_oMediator->notify_message(trUtf8("No destination file selected"), 5000);
			return;
		}

		if (m_oMediator->m_sExportUrl != exp->kurlrequester->url().url())
		{
			m_oMediator->m_sExportUrl = exp->kurlrequester->url().url();
			m_oMediator->set_dirty();
		}

		QImage l_oImage((int) l_oRect.width(), (int) l_oRect.height(), QImage::Format_RGB32);
		if (exp->m_oWidthC->isChecked())
		{
			l_oImage = l_oImage.scaledToWidth(exp->m_oWidth->value());
		}
		else
		{
			l_oImage = l_oImage.scaledToHeight(exp->m_oHeight->value());
		}
		l_oImage.fill(qRgb(255,255,255));
	
		QPainter l_oP;
		l_oP.begin(&l_oImage);
		l_oP.setRenderHints(QPainter::Antialiasing);
		scene()->render(&l_oP, l_oImage.rect(), l_oRect);
		l_oP.end();

		// TODO upload remote files
		QString path = exp->kurlrequester->url().toLocalFile();
		if (exp->kurlrequester->url().isRelative())
		{
			path = QDir::homePath() + notr("/") + path;
		}
		bool isOk = l_oImage.save(path);
		if (isOk)
			m_oMediator->notify_message(trUtf8("Exported '%1'").arg(path), 2000);
		else
			KMessageBox::sorry(this, trUtf8("Could not save to %1").arg(path), trUtf8("Missing picture"));
	}
}

void canvas_view::notify_export_doc()
{
	QRectF l_oRect = scene()->itemsBoundingRect();
	l_oRect = QRectF(l_oRect.topLeft() - QPointF(25, 25), l_oRect.bottomRight() + QPointF(25, 25));
	QRectF l_oR(0, 0, l_oRect.width(), l_oRect.height());

	// fill with white
	QImage l_oImage((int) l_oR.width(), (int) l_oR.height(), QImage::Format_RGB32);
	l_oImage.fill(qRgb(255,255,255));

	QPainter l_oP;
	l_oP.begin(&l_oImage);
	l_oP.setRenderHints(QPainter::Antialiasing);
	scene()->render(&l_oP, l_oR, l_oRect);
	l_oP.end();

	l_oImage.save(QString(m_oMediator->m_sTempDir + QString("/wholemap.png")));
}

QList<canvas_item*> canvas_view::selection() {
	// FIXME use scene()->selectedItems()
	QList<canvas_item*> ret;

	foreach (QGraphicsItem *tmp, items()) {
		if (tmp->type() == CANVAS_ITEM_T && tmp->isSelected()) {
			ret.append((canvas_item*) tmp);
		}
	}
	return ret;
}

void canvas_view::mouseMoveEvent(QMouseEvent *i_oEv)
{
	if (m_oRubberLine->isVisible())
	{
		QRect l_oSel = QRect(m_oLastPressPoint, i_oEv->pos());
		m_oRubberLine->setGeometry(l_oSel);
	}

	if (m_bScroll)
	{
		QScrollBar *h = horizontalScrollBar();
		QScrollBar *v = verticalScrollBar();
		QPoint d = i_oEv->pos() - m_oLastPressPoint;
		h->setValue(h->value() - d.x());
		v->setValue(v->value() - d.y());
		m_oLastPressPoint = i_oEv->pos();
		//QGraphicsView::mouseMoveEvent(i_oEv);
		return;
	}


	QGraphicsView::mouseMoveEvent(i_oEv);

	QList<QGraphicsItem*> sel = scene()->selectedItems();
	if (sel.size() > 4) { // does not solve the repainting problem
		QSet<canvas_link*> lst;
		foreach (QGraphicsItem*tmp, sel) {
			if (tmp->type() == CANVAS_ITEM_T && tmp->isSelected()) {
				canvas_item* x = (canvas_item*) tmp;
				foreach (canvas_link* l_oLink, x->m_oLinks) {
					lst.insert(l_oLink);
				}
			}
		}
		foreach (canvas_link* tmp, lst) {
			tmp->update_pos();
		}
	} else {
		foreach (QGraphicsItem*tmp, sel) {
			if (tmp->type() == CANVAS_ITEM_T && tmp->isSelected()) {
				((canvas_item*) tmp)->update_links();
			}
		}
	}
}

void canvas_view::notify_add_item(int id) {
	Q_ASSERT(! m_oItems.contains(id));
	canvas_item* l_oR = new canvas_item(this, id);
	m_oItems[id] = l_oR;
	l_oR->update_data();
	// do not call methods that create events here

	check_canvas_size();
}

void canvas_view::notify_change_data(int id)
{
	//canvas_item* l_oR = m_oItems.value(id);
	//qDebug()<<"FIXME data has changed!!!";
}

void canvas_view::notify_delete_item(int id) {
	m_bDeleting = true;
	canvas_item *l_oR1 = m_oItems.value(id);
	Q_ASSERT(l_oR1!=NULL);
	scene()->removeItem(l_oR1);
	m_oItems.remove(id);
	delete l_oR1;
	m_bDeleting = false;

	check_canvas_size();
}

void canvas_view::notify_link_items(int id1, int id2) {
	canvas_item *l_oR1 = m_oItems.value(id1);
	canvas_item *l_oR2 = m_oItems.value(id2);
	canvas_link * l_oLink = new canvas_link(this, l_oR1, l_oR2);
	l_oLink->update_pos();
	l_oR2->update();
}

void canvas_view::notify_unlink_items(int id1, int id2) {
	canvas_item *l_oR1 = m_oItems.value(id1);
	canvas_item *l_oR2 = m_oItems.value(id2);

	foreach (QGraphicsItem *l_oItem, items())
	{
		if (l_oItem->type() == CANVAS_LINK_T)
		{
			canvas_link *l_oLink = (canvas_link*) l_oItem;
			if (
				(l_oLink->m_oFrom == l_oR1 && l_oLink->m_oTo == l_oR2)
				||
				(l_oLink->m_oFrom == l_oR2 && l_oLink->m_oTo == l_oR1)
			   )
			{
				l_oLink->hide();
				l_oLink->rm_link();
				delete l_oLink;
				break;
			}
		}
	}
	l_oR1->update();
	l_oR2->update();
}

void canvas_view::notify_move(const QList<int>&sel, const QList<QPointF>&pos) {
	for (int i = 0; i < sel.size(); ++i) {
		m_oItems[sel[i]]->setPos(pos[i]);
		m_oItems[sel[i]]->update_links();
	}
}

void canvas_view::notify_repaint(int id) {
	m_oItems[id]->update();
}

void canvas_view::notify_edit(int id) {
	data_item *sel = *m_oMediator + id;
	if (m_oItems[id]->toPlainText() != sel->m_sSummary) {
		m_oItems[id]->setPlainText(sel->m_sSummary);
		m_oItems[id]->adjustSize();
		m_oItems[id]->update_links();
	}
	m_oItems[id]->update();
}

void canvas_view::notify_flag(int id) {
	m_oItems[id]->update_flags();
}

void canvas_view::notify_sort(int id) {
	show_sort(id, true); // TODO ITA
}

void canvas_view::notify_focus(void* ptr)
{
	bool cond = ptr == this;
	m_oAddItemAction->setEnabled(cond);
	m_oDeleteAction->setEnabled(cond);
	m_oEditAction->setEnabled(cond);
}

void canvas_view::focusInEvent(QFocusEvent *i_oEv)
{
	QGraphicsView::focusInEvent(i_oEv);
	m_oMediator->notify_focus(this);
}

void canvas_view::focusOutEvent(QFocusEvent *i_oEv)
{
	QGraphicsView::focusOutEvent(i_oEv);
	edit_off();
}

%: include  	"canvas_view.moc" 


void recurse_add(int id, QList<int>& sel, const QList<QPoint>& m_oLinks)
{
	sel.append(id);
	for (int i=0; i < m_oLinks.size(); i++)
	{
		QPoint l_oP = m_oLinks.at(i);
		if (l_oP.x() == id) recurse_add(l_oP.y(), sel, m_oLinks);
	}
}

void canvas_view::slot_select_subtree()
{
	QList<canvas_item*> seli = selection();
	Q_ASSERT(seli.size() == 1);

	mem_sel *sel = new mem_sel(m_oMediator);
	recurse_add(seli.at(0)->m_iId, sel->sel, m_oMediator->m_oLinks);
	sel->apply();
}

