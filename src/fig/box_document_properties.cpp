// Thomas Nagy 2007-2015 GPLV3

#include <QSpinBox>
#include <QComboBox>
#include <QTreeWidget>
#include <QTextEdit>
#include <QGridLayout>
#include <QLabel>
#include <QHeaderView>
#include <QDirModel>
#include <QCompleter>
#include <kfontdialog.h>
#include "mem_box.h"
#include "box_view.h"
#include "kiconloader.h"

#include <QtDebug>

#include "sem_mediator.h"
#include "box_document_properties.h"

box_document_properties::box_document_properties(box_view *i_oParent):
	KPageDialog(i_oParent)
{
	m_oView = i_oParent;

	setCaption(trUtf8("Diagram properties"));
	setButtons(KDialog::Ok | KDialog::Apply | KDialog::Cancel);
	//enableButtonApply(false);

	setFaceType(List);

	// TODO not the correct font
	data_item *l_oItem = m_oView->m_oMediator->m_oItems.value(m_oView->m_iId);
	m_oFontChooser = new KFontChooser(this, KFontChooser::NoDisplayFlags, QStringList(), 0);
	m_oFontChooser->setFont(l_oItem->m_oDiagramFont);
	KPageWidgetItem* l_oPage = new KPageWidgetItem(m_oFontChooser, trUtf8("Font"));
	l_oPage->setIcon(KIcon(DesktopIcon(notr("preferences-desktop-font"))));
	l_oPage->setHeader(trUtf8("Font for diagram objects"));
	addPage(l_oPage);

	connect(this, SIGNAL(applyClicked()), this, SLOT(apply()));
	connect(this, SIGNAL(okClicked()), this, SLOT(apply()));

	QSize size(650, 620);
	resize(size.expandedTo(minimumSizeHint()));
}

void box_document_properties::apply()
{
	data_item *l_oItem = m_oView->m_oMediator->m_oItems.value(m_oView->m_iId);
	mem_diagram_properties *mem = new mem_diagram_properties(m_oView->m_oMediator, m_oView->m_iId);
	mem->m_oOldFont = l_oItem->m_oDiagramFont;
	mem->m_oNewFont = m_oFontChooser->font();
	mem->apply();

	/*if (!isButtonEnabled(KDialog::Apply)) {
		return;
	}
	enableButtonApply(false);*/
}

#include "box_document_properties.moc"

