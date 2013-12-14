// Thomas Nagy 2013 GPLV3

#include <KUrl>
#include <QTreeView>
#include <QSortFilterProxyModel>

#ifndef FILETREE
#define FILETREE

class KDirModel;
class fileproxy : public QSortFilterProxyModel
{
	Q_OBJECT
	public:
	fileproxy(QObject*);

	bool lessThan(const QModelIndex &left, const QModelIndex &right) const;
	int columnCount(const QModelIndex& parent = QModelIndex()) const;
	bool filterAcceptsRow(int, const QModelIndex& i_oIndex) const;
};

class filetree : public QTreeView
{
	Q_OBJECT
	public:
		filetree(QWidget*);
		KDirModel *m_oModel;
		fileproxy *m_oProxy;

	public slots:
		void reclick(const QModelIndex& i_oModelIndex);

	signals:
		void url_selected(const KUrl&);
};

#endif
