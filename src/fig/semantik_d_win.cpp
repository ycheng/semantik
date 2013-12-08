// Thomas Nagy 2013 GPLV3

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
#include <KTabWidget>

#include "semantik_d_win.h"
#include "diagram_document.h"
//#include "sem_mediator.h"
//#include "box_view.h"
# include  "sembind.h"

semantik_d_win::semantik_d_win(QWidget *i_oParent) : KXmlGuiWindow(i_oParent)
{
	setObjectName("semantik_d_win");

	setWindowIcon(KIcon("semantik"));

	m_oTabWidget = new KTabWidget(this);
	m_oTabWidget->setCloseButtonEnabled(true);
	m_oTabWidget->setAutomaticResizeTabs(true);
	setCentralWidget(m_oTabWidget);
	m_oActiveDiagramView = NULL;

	connect(m_oTabWidget, SIGNAL(closeRequest(QWidget*)), this, SLOT(slot_remove_tab(QWidget*)));
	connect(m_oTabWidget, SIGNAL(currentChanged(int)), this, SLOT(slot_tab_changed(int)));

	KStandardAction::openNew(this, SLOT(slot_add_tab()), actionCollection());
	KStandardAction::quit(this, SLOT(close()), actionCollection());
	KStandardAction::save(this, NULL, actionCollection());
	KStandardAction::saveAs(this, NULL, actionCollection());
	KStandardAction::print(this, NULL, actionCollection());

	KStandardAction::open(this, SLOT(slot_open()), actionCollection());
	KStandardAction::tipOfDay(this, SLOT(slot_tip_of_day()), actionCollection());
	//m_oUndoAct = KStandardAction::undo(m_oMediator, SLOT(slot_undo()), actionCollection());
	//m_oUndoAct->setEnabled(false);
	//m_oRedoAct = KStandardAction::redo(m_oMediator, SLOT(slot_redo()), actionCollection());
	//m_oRedoAct->setEnabled(false);

	m_oRecentFilesAct = KStandardAction::openRecent(this, SLOT(slot_recent(const KUrl&)), actionCollection());

	setupGUI(QSize(800, 800), Default, notr("semantik/semantik-dui.rc"));

	read_config();
	statusBar()->showMessage(trUtf8("This is Semantik-d"), 2000);
	setAutoSaveSettings();
}

semantik_d_win::~semantik_d_win()
{

}

void semantik_d_win::wire_actions() {
	QAction *l_oTmp = actionCollection()->action(KStandardAction::name(KStandardAction::Save));
	if (m_oActiveDiagramView)
	{
		l_oTmp->setEnabled(true);
		connect(l_oTmp, SIGNAL(triggered()), m_oActiveDiagramView, SLOT(slot_save()));
	}
	else
	{
		l_oTmp->setEnabled(false);
	}

	l_oTmp = actionCollection()->action(KStandardAction::name(KStandardAction::SaveAs));
	if (m_oActiveDiagramView)
	{
		l_oTmp->setEnabled(true);
		connect(l_oTmp, SIGNAL(triggered()), m_oActiveDiagramView, SLOT(slot_save_as()));
	}
	else
	{
		l_oTmp->setEnabled(false);
	}

	l_oTmp = actionCollection()->action(KStandardAction::name(KStandardAction::Print));
	if (m_oActiveDiagramView)
	{
		l_oTmp->setEnabled(true);
		connect(l_oTmp, SIGNAL(triggered()), m_oActiveDiagramView, SLOT(slot_print()));
	}
	else
	{
		l_oTmp->setEnabled(false);
	}
}

void semantik_d_win::slot_add_tab()
{
	m_oActiveDiagramView = new diagram_document(m_oTabWidget);
	m_oActiveDiagramView->init();
	int l_iIndex = m_oTabWidget->addTab(m_oActiveDiagramView, trUtf8("[Untitled]"));
	m_oTabWidget->setCurrentIndex(l_iIndex);
	wire_actions();
}

void semantik_d_win::slot_remove_tab(QWidget* widget)
{
	m_oActiveDiagramView->disconnect();
	delete m_oActiveDiagramView;
	m_oActiveDiagramView = NULL;
	slot_tab_changed(m_oTabWidget->currentIndex());
	wire_actions();
}

void semantik_d_win::slot_tab_changed(int i_iIndex)
{
	if (m_oActiveDiagramView != NULL) {
		m_oActiveDiagramView->disconnect();
	}
	m_oActiveDiagramView = static_cast<diagram_document*>(m_oTabWidget->currentWidget());
	wire_actions();
}

void semantik_d_win::read_config()
{
	KConfigGroup l_oConfig(KGlobal::config(), notr("General Options"));
	m_oRecentFilesAct->loadEntries(KGlobal::config()->group(notr("Recent Files")));
	move(l_oConfig.readEntry(notr("winpos"), QPoint(0, 0)));
	//m_oMediator->m_sOutDir = l_oConfig.readEntry(notr("outdir"), notr("/tmp/"));
	//bind_node::set_var(notr("outdir"), m_oMediator->m_sOutDir);
}

void semantik_d_win::write_config()
{
	KConfigGroup l_oConfig(KGlobal::config(), notr("General Options"));
	m_oRecentFilesAct->saveEntries(KGlobal::config()->group(notr("Recent Files")));
	//l_oConfig.writeEntry(notr("winpos"), pos());
	//l_oConfig.writeEntry(notr("outdir"), bind_node::get_var(notr("outdir")));
	l_oConfig.sync();
}

bool semantik_d_win::queryClose()
{
	write_config();
	//if (!m_oMediator->m_bDirty) return true;
	//return proceed_save();
	return true;
}

void semantik_d_win::update_title() {
}

void semantik_d_win::slot_open() {
}

void semantik_d_win::slot_recent(const KUrl &) {
}

void semantik_d_win::slot_enable_undo(bool, bool) {
}

void semantik_d_win::slot_properties() {
}

void semantik_d_win::slot_generate() {
}

void semantik_d_win::slot_tip_of_day() {
}

void semantik_d_win::slot_message(const QString &, int) {
}

#include "semantik_d_win.moc"


