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

#include "semantik_d_win.h"
#include "sem_mediator.h"
#include "box_view.h"

semantik_d_win::semantik_d_win(QWidget *i_oParent) : KXmlGuiWindow(i_oParent)
{
	setObjectName("semantik_win");

	setWindowIcon(KIcon("semantik"));

	m_oMediator = new sem_mediator(this);

	QFrame *fr = new QFrame(this);
        fr->setLineWidth(0);
        fr->setFrameStyle(QFrame::NoFrame);
        m_oDiagramView = new box_view(fr, m_oMediator);
        setCentralWidget(fr);
        QGridLayout *ll = new QGridLayout(fr);
        ll->addWidget(m_oDiagramView);

	KStandardAction::quit(this, SLOT(close()), actionCollection());
	KStandardAction::save(this, SLOT(slot_save()), actionCollection());
	KStandardAction::saveAs(this, SLOT(slot_save_as()), actionCollection());
	KStandardAction::open(this, SLOT(slot_open()), actionCollection());
	KStandardAction::print(this, SLOT(slot_print()), actionCollection());
	KStandardAction::tipOfDay(this, SLOT(slot_tip_of_day()), actionCollection());
	m_oUndoAct = KStandardAction::undo(m_oMediator, SLOT(slot_undo()), actionCollection());
	m_oUndoAct->setEnabled(false);
	m_oRedoAct = KStandardAction::redo(m_oMediator, SLOT(slot_redo()), actionCollection());
	m_oRedoAct->setEnabled(false);

	m_oRecentFilesAct = KStandardAction::openRecent(this, SLOT(slot_recent(const KUrl&)), actionCollection());


	setXMLFile(notr("semantikui.rc"));
        setupGUI();
}

semantik_d_win::~semantik_d_win()
{

}

#include "semantik_d_win.moc"

