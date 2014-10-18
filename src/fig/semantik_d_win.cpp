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
#include <QDockWidget>
#include <KMessageBox>
#include <ktip.h>
#include <QFrame>
#include <KTabWidget>
#include <KDirModel>

#include "semantik_d_win.h"
#include "diagram_document.h"
#include "sem_mediator.h"
#include "box_view.h"
# include  "sembind.h"
#include  "filetree.h"

semantik_d_win::semantik_d_win(QWidget *i_oParent) : KXmlGuiWindow(i_oParent)
{
	setObjectName("semantik_d_win");

	setWindowIcon(KIcon("semantik"));

	m_oTabWidget = new KTabWidget(this);
	m_oTabWidget->setCloseButtonEnabled(true);
	m_oTabWidget->setAutomaticResizeTabs(true);
	setCentralWidget(m_oTabWidget);
	m_oActiveDocument = NULL;

	connect(m_oTabWidget, SIGNAL(closeRequest(QWidget*)), this, SLOT(slot_remove_tab(QWidget*)));
	connect(m_oTabWidget, SIGNAL(currentChanged(int)), this, SLOT(slot_tab_changed(int)));

	KStandardAction::openNew(this, SLOT(slot_add_tab()), actionCollection());
	KStandardAction::quit(this, SLOT(close()), actionCollection());
	KStandardAction::save(this, NULL, actionCollection());
	KStandardAction::saveAs(this, NULL, actionCollection());
	KStandardAction::print(this, NULL, actionCollection());

	KStandardAction::copy(this, NULL, actionCollection());

	KStandardAction::open(this, SLOT(slot_open()), actionCollection());
	KStandardAction::tipOfDay(this, SLOT(slot_tip_of_day()), actionCollection());
	m_oUndoAct = KStandardAction::undo(this, NULL, actionCollection());
	m_oUndoAct->setEnabled(false);
	m_oRedoAct = KStandardAction::redo(this, NULL, actionCollection());
	m_oRedoAct->setEnabled(false);

	m_oRecentFilesAct = KStandardAction::openRecent(this, SLOT(slot_recent(const KUrl&)), actionCollection());

	QDockWidget *l_oDock = new QDockWidget(trUtf8("Files"), this);
	l_oDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);
	addDockWidget(Qt::LeftDockWidgetArea, l_oDock);
	l_oDock->setObjectName(notr("VarsDock"));
	actionCollection()->addAction(notr("show_dock_files"), l_oDock->toggleViewAction());
	m_oFileTree = new filetree(l_oDock);
	l_oDock->setWidget(m_oFileTree);

	read_config();
	setAutoSaveSettings();

	setupGUI(QSize(1000, 800), Default, notr("semantik/semantik-dui.rc"));
	statusBar()->showMessage(trUtf8("This is Semantik-d"), 2000);

	connect(m_oFileTree, SIGNAL(url_selected(const KUrl&)), this, SLOT(slot_recent(const KUrl&)));
	connect(this, SIGNAL(url_opened(const KUrl&)), this, SLOT(record_open_url(const KUrl&)));

	m_oFileTree->m_oModel->expandToUrl(KUrl("~"));
}

void semantik_d_win::wire_actions()
{
	QAction *l_oTmp = actionCollection()->action(KStandardAction::name(KStandardAction::Save));
	l_oTmp->disconnect();
	if (m_oActiveDocument)
	{
		l_oTmp->setEnabled(true);
		connect(l_oTmp, SIGNAL(triggered()), m_oActiveDocument->m_oDiagramView, SLOT(slot_save()));
	}
	else
	{
		l_oTmp->setEnabled(false);
	}

	l_oTmp = actionCollection()->action(KStandardAction::name(KStandardAction::SaveAs));
	l_oTmp->disconnect();
	if (m_oActiveDocument)
	{
		l_oTmp->setEnabled(true);
		connect(l_oTmp, SIGNAL(triggered()), m_oActiveDocument->m_oDiagramView, SLOT(slot_export_to_file()));
	}
	else
	{
		l_oTmp->setEnabled(false);
	}

	l_oTmp = actionCollection()->action(KStandardAction::name(KStandardAction::Print));
	l_oTmp->disconnect();
	if (m_oActiveDocument)
	{
		l_oTmp->setEnabled(true);
		connect(l_oTmp, SIGNAL(triggered()), m_oActiveDocument->m_oDiagramView, SLOT(slot_print()));
	}
	else
	{
		l_oTmp->setEnabled(false);
	}

	l_oTmp = actionCollection()->action(KStandardAction::name(KStandardAction::Undo));
	l_oTmp->disconnect();
	if (m_oActiveDocument)
	{
		l_oTmp->setEnabled(true);
		connect(l_oTmp, SIGNAL(triggered()), m_oActiveDocument->m_oMediator, SLOT(slot_undo()));
	}
	else
	{
		l_oTmp->setEnabled(false);
	}

	l_oTmp = actionCollection()->action(KStandardAction::name(KStandardAction::Redo));
	l_oTmp->disconnect();
	if (m_oActiveDocument)
	{
		l_oTmp->setEnabled(true);
		connect(l_oTmp, SIGNAL(triggered()), m_oActiveDocument->m_oMediator, SLOT(slot_redo()));
	}
	else
	{
		l_oTmp->setEnabled(false);
	}

	l_oTmp = actionCollection()->action(KStandardAction::name(KStandardAction::Copy));
	l_oTmp->disconnect();
	if (m_oActiveDocument)
	{
		l_oTmp->setEnabled(true);
		connect(l_oTmp, SIGNAL(triggered()), m_oActiveDocument->m_oDiagramView, SLOT(slot_copy_picture()));
	}
	else
	{
		l_oTmp->setEnabled(false);
	}

	if (m_oActiveDocument)
	{
		connect(m_oActiveDocument->m_oDiagramView, SIGNAL(sig_message(const QString&, int)), statusBar(), SLOT(showMessage(const QString&, int)));
		connect(m_oActiveDocument, SIGNAL(sig_tab_name(diagram_document*, const KUrl&)), this, SLOT(slot_update_tab_text(diagram_document*, const KUrl&)));
		connect(m_oActiveDocument->m_oMediator, SIGNAL(enable_undo(bool, bool)), this, SLOT(slot_enable_undo(bool, bool)));

		m_oActiveDocument->m_oMediator->check_undo(true);

		connect(m_oActiveDocument->m_oMediator, SIGNAL(update_title()), this, SLOT(update_title()));
		update_title();
	}
}

void semantik_d_win::slot_enable_undo(bool i_bUndo, bool i_bRedo)
{
	actionCollection()->action(KStandardAction::name(KStandardAction::Undo))->setEnabled(i_bUndo);
	actionCollection()->action(KStandardAction::name(KStandardAction::Redo))->setEnabled(i_bRedo);
}

void semantik_d_win::slot_add_tab()
{
	m_oActiveDocument = new diagram_document(m_oTabWidget);
	m_oActiveDocument->init();
	int l_iIndex = m_oTabWidget->addTab(m_oActiveDocument, trUtf8("[Untitled]"));
	m_oTabWidget->setCurrentIndex(l_iIndex);
	wire_actions();
}

void semantik_d_win::slot_remove_tab(QWidget* i_oWidget)
{
	if (save_tab(i_oWidget))
	{
		int l_iIdx = m_oTabWidget->indexOf(i_oWidget);
		m_oTabWidget->removeTab(l_iIdx);
		delete i_oWidget;
		// slot_tab_changed
	}
}

void semantik_d_win::slot_tab_changed(int i_iIndex)
{
	m_oActiveDocument = static_cast<diagram_document*>(m_oTabWidget->currentWidget());
	if (m_oActiveDocument != NULL)
	{
		emit url_opened(m_oActiveDocument->m_oDiagramView->m_oCurrentUrl);
	}
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
	l_oConfig.writeEntry(notr("winpos"), pos());
	//l_oConfig.writeEntry(notr("outdir"), bind_node::get_var(notr("outdir")));
	l_oConfig.sync();
}

bool semantik_d_win::queryClose()
{
	write_config();
	for (int i = 0; i < m_oTabWidget->count(); ++i)
	{
		if (!save_tab(m_oTabWidget->widget(i)))	{
			return false;
		}
	}
	return true;
}

bool semantik_d_win::save_tab(QWidget *i_oWidget) {
	diagram_document *l_oDoc = static_cast<diagram_document*>(i_oWidget);
	if (l_oDoc->m_oMediator->m_bDirty)
	{
		QString l_oTitle = l_oDoc->m_oMediator->m_sLastSaved;
		if (l_oTitle.isEmpty()) l_oTitle = trUtf8("Untitled");

		int l_o = KMessageBox::warningYesNoCancel(NULL, //this,
			trUtf8("The document \"%1\" has been modified.\nDo you want to save your changes or discard them?").arg(l_oTitle),
			trUtf8("Close Document"),
		KStandardGuiItem::save(), KStandardGuiItem::discard());

		switch (l_o)
		{
			case KMessageBox::Yes:
			{
				if (!l_oDoc->m_oDiagramView->slot_save()) return false;
			}
			case KMessageBox::No:
				return true;
			default:
				return false;
		}
	}
	return true;
}

void semantik_d_win::slot_update_tab_text(diagram_document* i_oDoc, const KUrl & i_oUrl) {
	int l_iIndex = m_oTabWidget->indexOf(i_oDoc);
	if (l_iIndex > -1)
	{
		m_oTabWidget->setTabText(l_iIndex, i_oUrl.fileName());
	}
	emit url_opened(i_oUrl);
}

void semantik_d_win::slot_open()
{
	KUrl l_o = KFileDialog::getOpenUrl(KUrl(notr("kfiledialog:///document")),
                trUtf8("*.semd|Semantik diagram (*.semd)"), this,
                trUtf8("Choose a file to open"));
	if (l_o.isEmpty()) return;

	// file already open in a tab
	for (int i = 0; i < m_oTabWidget->count(); ++i)
	{
		diagram_document *l_oDoc = static_cast<diagram_document*>(m_oTabWidget->widget(i));
		if (l_oDoc->m_oDiagramView->m_oCurrentUrl.equals(l_o))
		{
			m_oTabWidget->setCurrentWidget(l_oDoc);
			emit url_opened(m_oActiveDocument->m_oDiagramView->m_oCurrentUrl);
			return;
		}
	}

	// just open a new tab
	diagram_document *l_oTmp = m_oActiveDocument;
	m_oActiveDocument = new diagram_document(m_oTabWidget);
	m_oActiveDocument->init();
	if (m_oActiveDocument->m_oDiagramView->import_from_file(l_o))
	{
		int l_iIndex = m_oTabWidget->addTab(m_oActiveDocument, m_oActiveDocument->m_oDiagramView->m_oCurrentUrl.fileName());
		m_oTabWidget->setCurrentIndex(l_iIndex);
		m_oActiveDocument->m_oMediator->m_oUndoStack.clear();
		m_oActiveDocument->m_oMediator->set_dirty(false);
		wire_actions();
		emit url_opened(m_oActiveDocument->m_oDiagramView->m_oCurrentUrl);
	}
	else
	{
		delete m_oActiveDocument;
		m_oActiveDocument = l_oTmp;
	}
}

void semantik_d_win::slot_recent(const KUrl& i_oUrl)
{
	if (i_oUrl.isEmpty()) return;

	// file already open in a tab
	for (int i = 0; i < m_oTabWidget->count(); ++i)
	{
		diagram_document *l_oDoc = static_cast<diagram_document*>(m_oTabWidget->widget(i));
		if (l_oDoc->m_oDiagramView->m_oCurrentUrl.equals(i_oUrl))
		{
			m_oTabWidget->setCurrentWidget(l_oDoc);
			emit url_opened(m_oActiveDocument->m_oDiagramView->m_oCurrentUrl);
			return;
		}
	}

	diagram_document *l_oTmp = m_oActiveDocument;
	m_oActiveDocument = new diagram_document(m_oTabWidget);
	m_oActiveDocument->init();
	if (m_oActiveDocument->m_oDiagramView->import_from_file(i_oUrl))
	{
		int l_iIndex = m_oTabWidget->addTab(m_oActiveDocument, m_oActiveDocument->m_oDiagramView->m_oCurrentUrl.fileName());
		m_oTabWidget->setCurrentIndex(l_iIndex);
		emit url_opened(m_oActiveDocument->m_oDiagramView->m_oCurrentUrl);
		m_oActiveDocument->m_oMediator->m_oUndoStack.clear();
		m_oActiveDocument->m_oMediator->set_dirty(false);
		wire_actions();
		emit url_opened(m_oActiveDocument->m_oDiagramView->m_oCurrentUrl);
	}
	else
	{
		delete m_oActiveDocument;
		m_oActiveDocument = l_oTmp;
	}
}

void semantik_d_win::record_open_url(const KUrl & i_oUrl)
{
	if (!i_oUrl.isValid()) return;
	m_oRecentFilesAct->addUrl(i_oUrl);
	m_oFileTree->m_oModel->expandToUrl(i_oUrl);
}

void semantik_d_win::slot_properties() {
}

void semantik_d_win::slot_generate() {
}

void semantik_d_win::slot_tip_of_day() {
}

void semantik_d_win::print_current(KUrl i_oUrl)
{
	QPair<int, int> p;
	if (m_oActiveDocument != NULL)
	{
		m_oActiveDocument->m_oDiagramView->batch_print_map(i_oUrl.path(), p);
	}
}

void semantik_d_win::update_title() {
	QString mod;
	if (m_oActiveDocument->m_oMediator->m_bDirty) mod = trUtf8(" [Modified] ");

	QString txt;
	if (m_oActiveDocument->m_oMediator->m_oCurrentUrl.path().isEmpty())
	{
		txt = trUtf8("Semantik Diagram %1").arg(mod);
	}
	else
	{
		txt = trUtf8("%1 %2 - Semantik Diagram").arg(m_oActiveDocument->m_oMediator->m_oCurrentUrl.path(), mod);
		//m_oRecentFilesAct->addUrl(m_oMediator->m_oCurrentUrl); // TODO
	}
	setWindowTitle(txt);
}

#include "semantik_d_win.moc"


