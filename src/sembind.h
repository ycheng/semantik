// Thomas Nagy 2007-2011 GPLV3

#ifndef BIND_NODE_H
#define BIND_NODE_H

#include <QString>
#include <QList>

class data_item;
class bind_node
{
	public:
		static bind_node* instance();

		static QString get_var(const QString &);
		static void set_var(const QString &, const QString &);

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

		bind_node();
		~bind_node();
		static QMap<QString, QString> s_oVars;
		static bind_node* _root;
		QList<bind_node*> _children;
		data_item *m_oItem;
};

int init_py();

#endif // BIND_NODE_H

