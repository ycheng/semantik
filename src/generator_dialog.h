// Thomas Nagy 2007-2012 GPLV3

#ifndef GENERATOR_DIALOG_H
#define GENERATOR_DIALOG_H

#include <QDialog>

#include "con.h"
#include "template_entry.h"

class QLineEdit;
class QTextEdit;
class QTreeWidgetItem;
class generator_dialog : public KDialog
{
	Q_OBJECT
	public:
		generator_dialog(QWidget *i_oParent, QList<template_entry> i_oLst);
		QList<template_entry> m_oLst;
		QLineEdit *m_oLocation;
		QLineEdit *m_oName;
		QTextEdit *m_oTextEdit;
		QString m_sCurrent;
		QTreeWidget *m_oTree;
		QString m_sShortName;

	public slots:
		void selection_changed(QTreeWidgetItem *i_o, int i_oCol);
		void slotButtonClicked(int button);
		void activate_from_name(const QString &i_s);
};

#endif

