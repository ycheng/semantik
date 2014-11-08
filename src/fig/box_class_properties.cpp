// Thomas Nagy 2007-2014 GPLV3

#include <KMessageBox>
#include <QSpinBox>
#include <QComboBox>
#include <QTreeWidget>
#include <QTextEdit>
#include <QGridLayout>
#include <QLabel>
#include <QHeaderView>
#include <QDirModel>
#include <QCompleter>
#include <QLineEdit>
#include <QTextEdit>
#include <QRegExp>
#include "mem_box.h"
#include "box_view.h"

#include <QtDebug>

#include "box_class_properties.h"

box_class_properties::box_class_properties(QWidget *i_oParent, box_class *i_oClass):
	KDialog(i_oParent), m_oClass(i_oClass)
{
	QWidget *widget = new QWidget(this);
	setCaption(trUtf8("Class properties"));
        setButtons(KDialog::Ok | KDialog::Apply | KDialog::Cancel);
	enableButtonApply(false);
	m_oClass = i_oClass;

	QGridLayout *l_oGridLayout = new QGridLayout(widget);

	m_oClassDefinition = new QTextEdit(widget);
	l_oGridLayout->addWidget(m_oClassDefinition, 0, 0);
	m_oClassDefinition->setStyleSheet("font-family: \"Courier New\", Courier, monospace;");

        setMainWidget(widget);
        QSize size(700, 320);
        resize(size.expandedTo(minimumSizeHint()));

	connect(m_oClassDefinition, SIGNAL(textChanged()), this, SLOT(enable_apply()));

        connect(this, SIGNAL(applyClicked()), this, SLOT(apply()));
        connect(this, SIGNAL(okClicked()), this, SLOT(apply()));

	m_oClassDefinition->setFocus();
}

void box_class_properties::enable_apply(int) {
	enableButtonApply(true);
}

void box_class_properties::enable_apply() {
	enableButtonApply(true);
}

void box_class_properties::apply() {
	if (!isButtonEnabled(KDialog::Apply)) {
		return;
	}

	QStringList l_oTmp = m_oClassDefinition->toPlainText().split(notr("\n"));

	if (l_oTmp.size() < 1) {
		KMessageBox::sorry(this, trUtf8("No class name given"), trUtf8("Missing class name"));
		return;
	}

	QRegExp rx("(static|abstract)?\\s*(static|abstract)?\\s*class\\s+(\\w.*)");
	if (rx.indexIn(l_oTmp[0]) != -1) {
		mem_class *mem = new mem_class(m_oClass->m_oView->m_oMediator, m_oClass->m_oView->m_iId);
		mem->init(m_oClass->m_oBox);

		mem->m_oNewBox.m_sText = rx.cap(3);
		mem->m_oNewBox.m_bStatic = (notr("static") == rx.cap(1) || notr("static") == rx.cap(2));
		mem->m_oNewBox.m_bAbstract = (notr("abstract") == rx.cap(1) || notr("abstract") == rx.cap(2));
		mem->m_oNewBox.m_oMethods.clear();
		mem->m_oNewBox.m_oAttributes.clear();

		QRegExp rm("(public|private|protected|package|derived)?\\s*(static|abstract)?\\s*(static|abstract)?\\s*(\\w.*)");
		QRegExp rs("stereotype\\s*(.*)");
		for (int i=1; i < l_oTmp.size() - 1; ++i) {
			QString l_s = l_oTmp[i];
			if (rm.indexIn(l_s) >= 0) {
				QString l_sData = rm.cap(4);
				bool l_bStatic = (notr("static") == rm.cap(3) || notr("static") == rm.cap(2));
				bool l_bAbstract = (notr("abstract") == rm.cap(3) || notr("abstract") == rm.cap(2));

				if (l_sData.contains(notr("(")))
				{
					data_box_method m;
					m.m_bStatic = l_bStatic;
					m.m_bAbstract = l_bAbstract;
					m.m_sText = rm.cap(4);
					m.m_oVisibility = visibility::toVisibility(rm.cap(1));
					mem->m_oNewBox.m_oMethods.push_back(m);
				}
				else
				{
					data_box_attribute a;
					a.m_bStatic = l_bStatic;
					a.m_sText = rm.cap(4);
					a.m_oVisibility = visibility::toVisibility(rm.cap(1));
					mem->m_oNewBox.m_oAttributes.push_back(a);
				}
			} else if (rs.indexIn(l_s) >= 0) {
				mem->m_oNewBox.m_sStereotype = rs.cap(1);
			}
		}
		mem->apply();
		enableButtonApply(false);
	} else {
		qDebug()<<"no match for"<<l_oTmp[0];
		return;
	}
}

#include "box_class_properties.moc"

