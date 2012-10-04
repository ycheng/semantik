// Thomas Nagy 2007-2012 GPLV3

#include <QVariant>
#include <QAction>
#include <QApplication>
#include <QButtonGroup>
#include <KDialog>
#include <QGridLayout>
#include<QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QSpacerItem>
#include<QCoreApplication>
#include <QTreeWidget>
#include <QTextEdit>
#include <QHeaderView>
#include <QDirModel>
#include <QCompleter>
#include <KMessageBox>

#include <QtDebug>

#include "generator_dialog.h"

generator_dialog::generator_dialog(QWidget *i_oParent, QList<template_entry> i_oLst):
	KDialog(i_oParent),
	m_oLst(i_oLst)
{
	QWidget *widget = new QWidget(this);
	setCaption(trUtf8("Generate documents"));
	setButtons(KDialog::Ok | KDialog::Cancel);

	QGridLayout *l_oGridLayout = new QGridLayout(widget);

	QLabel *label = new QLabel(widget);
	label->setText(trUtf8("Generators"));

	QLabel *label2 = new QLabel(widget);
	label2->setText(trUtf8("Description"));

	l_oGridLayout->addWidget(label, 0, 0, 1, 2);
	l_oGridLayout->addWidget(label2, 0, 2, 1, 1);

	m_oTree = new QTreeWidget(widget);
	m_oTree->header()->hide();
	l_oGridLayout->addWidget(m_oTree, 1, 0, 1, 2);
	m_oTree->setFocusPolicy(Qt::NoFocus);

	m_oTextEdit = new QTextEdit(widget);
	m_oTextEdit->setReadOnly(true);
	l_oGridLayout->addWidget(m_oTextEdit, 1, 2, 1, 1);
	m_oTextEdit->setFocusPolicy(Qt::NoFocus);


	label = new QLabel(widget);
	label->setText(trUtf8("Name"));
	l_oGridLayout->addWidget(label, 2, 0, 1, 1);

	m_oName = new QLineEdit(widget);
	l_oGridLayout->addWidget(m_oName, 2, 1, 1, 2);

	label = new QLabel(widget);
	label->setText(trUtf8("Location"));
	l_oGridLayout->addWidget(label, 3, 0, 1, 1);

	m_oLocation = new QLineEdit(widget);
	l_oGridLayout->addWidget(m_oLocation, 3, 1, 1, 2);
        QCompleter *l_oCompleter = new QCompleter();
        QDirModel *l_oDirModel = new QDirModel(l_oCompleter);
        l_oDirModel->setFilter(QDir::AllDirs | QDir::Files | QDir::CaseSensitive | QDir::NoDotAndDotDot);
        l_oDirModel->setNameFilters(QStringList()<<"*.jpg"<<"*.png"<<"*.JPG"<<"*.PNG"<<"*.jpeg"<<"*.JPEG");
        l_oCompleter->setModel(l_oDirModel);
        m_oLocation->setCompleter(l_oCompleter);


	foreach (template_entry l_o, m_oLst)
	{
		QTreeWidgetItem *l_oItem = new QTreeWidgetItem(m_oTree);
		l_oItem->setText(0, l_o.m_sName);
		l_oItem->setData(0, Qt::UserRole+3, l_o.m_sFile);
		l_oItem->setData(0, Qt::UserRole+4, l_o.m_sTip);
	}

	connect(m_oTree, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this, SLOT(selection_changed(QTreeWidgetItem *, int)));


   	setMainWidget(widget);

	QSize size(421, 250);
	size = size.expandedTo(minimumSizeHint());
	resize(size);
}

void generator_dialog::selection_changed(QTreeWidgetItem * i_oItem, int i_oCol)
{
	if (!i_oItem)
	{
		m_oTextEdit->clear();
	}
	else
	{
		m_oTextEdit->setText(i_oItem->data(0, Qt::UserRole+4).toString());
		m_sCurrent = i_oItem->data(0, Qt::UserRole+3).toString();
		m_sShortName = i_oItem->text(0);
	}

}

void generator_dialog::slotButtonClicked(int button) {
	if (button == KDialog::Ok)
	{
		if (m_sCurrent.length()<2)
		{
			KMessageBox::sorry(this, trUtf8("No document generator chosen."), trUtf8("Missing data"));
			return;
		}
		QFileInfo l_oFi(m_oLocation->text());
		if (!l_oFi.isDir() || !l_oFi.isWritable())
		{
			KMessageBox::sorry(this, trUtf8("Invalid output directory %1").arg(m_oLocation->text()), trUtf8("Missing location"));
			return;
		}
		accept();
	}
	else
	{
		KDialog::slotButtonClicked(button);
	}
}

void generator_dialog::activate_from_name(const QString &i_s)
{
	QList<QTreeWidgetItem *> l_o = m_oTree->findItems(i_s, Qt::MatchFixedString, 0);
	if (l_o.size() == 1)
	{
		m_sShortName = i_s;
		m_oTree->setCurrentItem(l_o[0]);
		selection_changed(l_o[0], 0);
	}
}

#include "generator_dialog.moc"
