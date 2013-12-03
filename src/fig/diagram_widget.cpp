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

#include "diagram_widget.h"
#include "sem_mediator.h"
#include "box_view.h"
# include  "sembind.h"

diagram_widget::diagram_widget(QWidget *i_oParent) : QFrame(i_oParent)
{
        setLineWidth(0);
        setFrameStyle(QFrame::NoFrame);

	m_oMediator = new sem_mediator(this);
        m_oDiagramView = new box_view(this, m_oMediator);

        QGridLayout *ll = new QGridLayout(this);
        ll->addWidget(m_oDiagramView);
}

diagram_widget::~diagram_widget()
{

}

void diagram_widget::slot_open() {
}

bool diagram_widget::slot_save() {
	return true;
}

bool diagram_widget::slot_save_as() {
	return true;
}

void diagram_widget::slot_print() {
}

#include "diagram_widget.moc"


