// Thomas Nagy 2007-2015 GPLV3

#ifndef BIND_NODE_H
#define BIND_NODE_H

#include <QString>
#include <QList>
#include <QHash>

class data_item;
class sem_mediator;
class bind_node
{
	public:
		static bind_node* instance();

		static QString get_var(const QString &);
		static void set_var(const QString &, const QString &);
		static void set_result(const QString &, const QString &);

		static QString get_item_ids();
		static bind_node* get_item_by_id(int i);

		// tree manipulation
		int child_count();
		bind_node* child_num(int i);

		// summary, text, comment, ..
		QString get_val(const QString &);
		void set_val(const QString &, const QString &);

		// links
		int link_count();
		QString link_location(int);
		QString link_caption(int);

		// table
		int num_rows();
		int num_cols();
		QString tbl_cell(int row, int col);

		// utils
		static QString protectXML(const QString &);
		static QString protectHTML(const QString &);

	//protected:

		static void init(sem_mediator*);
		static bind_node* create_tree(sem_mediator *model, int i_i);
		bind_node();
		~bind_node();
		static QMap<QString, QString> s_oVars;
		static QMap<QString, QString> s_oResults;
		static bind_node* _root;
		static QHash<int, bind_node*> _cache;
		static sem_mediator* _model;
		QList<bind_node*> _children;
		data_item *m_oItem;
};

int init_py();

#endif // BIND_NODE_H

