// Thomas Nagy 2013-2014 GPLV3

#include <QtGui>
#include <QFile>
#include <QTextBrowser>
#include <QProgressDialog>
#include <KStatusBar>
#include <KFileDialog>
#include <KConfigGroup>
#include <KMenuBar>
#include <KApplication>
#include <KStandardAction>
#include <KRecentFilesAction>
#include <KActionCollection>
#include<KToolBar>
#include <KAction>
#include <KMenu>
#include <KMessageBox>
#include <ktip.h>
#include <QFrame>

#include "diagram_document.h"
#include "sem_mediator.h"
#include "box_view.h"
# include  "sembind.h"
#include "mem_base.h"

diagram_document::diagram_document(QWidget *i_oParent) : QFrame(i_oParent)
{
        setLineWidth(0);
        setFrameStyle(QFrame::NoFrame);

	m_oMediator = new sem_mediator(this);
        m_oDiagramView = new box_view(this, m_oMediator);

        QGridLayout *ll = new QGridLayout(this);
        ll->addWidget(m_oDiagramView);
}

void diagram_document::init()
{
	connect(m_oMediator, SIGNAL(sig_add_item(int)), m_oDiagramView, SLOT(notify_add_item(int)));
	connect(m_oMediator, SIGNAL(sig_select(const QList<int>&, const QList<int>&)), m_oDiagramView, SLOT(notify_select(const QList<int>&, const QList<int>&)));

	connect(m_oMediator, SIGNAL(sig_add_box(int, int)), m_oDiagramView, SLOT(notify_add_box(int, int)));
	connect(m_oMediator, SIGNAL(sig_del_box(int, int)), m_oDiagramView, SLOT(notify_del_box(int, int)));
	connect(m_oMediator, SIGNAL(sig_edit_box(int, int)), m_oDiagramView, SLOT(notify_edit_box(int, int)));
	connect(m_oMediator, SIGNAL(sig_link_box(int, data_link*)), m_oDiagramView, SLOT(notify_link_box(int, data_link*)));
	connect(m_oMediator, SIGNAL(sig_unlink_box(int, data_link*)), m_oDiagramView, SLOT(notify_unlink_box(int, data_link*)));
	connect(m_oMediator, SIGNAL(sig_change_link_box(int, data_link*)), m_oDiagramView, SLOT(notify_change_link_box(int, data_link*)));
	connect(m_oMediator, SIGNAL(sig_box_props(int, const QList<diagram_item*>&)), m_oDiagramView, SLOT(notify_box_props(int, const QList<diagram_item*>&)));
	connect(m_oMediator, SIGNAL(sig_pos_box(int, const QList<data_box*>&)), m_oDiagramView, SLOT(notify_pos_box(int, const QList<data_box*>&)));
	connect(m_oMediator, SIGNAL(sig_size_box(int, const QList<data_box*>&)), m_oDiagramView, SLOT(notify_size_box(int, const QList<data_box*>&)));
	connect(m_oMediator, SIGNAL(sig_focus(void *)), m_oDiagramView, SLOT(notify_focus(void *)));

	connect(m_oDiagramView, SIGNAL(sig_Url(const KUrl&)), this, SLOT(slot_tab_name(const KUrl&)));

	mem_add *add = new mem_add(m_oMediator);
	add->init();
	add->item->m_iXX = 0;
	add->item->m_iYY = 0;
	add->parent = NO_ITEM;
	add->apply();

	m_oMediator->m_oUndoStack.clear();
	m_oMediator->m_oRedoStack.clear();

	m_oMediator->notify_focus(m_oDiagramView);
}

diagram_document::~diagram_document()
{
	m_oMediator->disconnect();
	delete m_oMediator;
	delete m_oDiagramView;
}

void diagram_document::slot_tab_name(const KUrl& i_oUrl)
{
	emit sig_tab_name(this, i_oUrl);
}

void diagram_document::slot_open() {
}

void diagram_document::update_title() {

}

#include "diagram_document.moc"


