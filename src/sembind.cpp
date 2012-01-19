// Thomas Nagy 2007-2012 GPLV3

#include <QPair>
#include <QtDebug>
#include <QXmlDefaultHandler>

#include "data_item.h"
 #inclu\
de "sem_mediator.h"
#include  "con.h"
#include "sembind.h"
#include "sem_mediator.h"

bind_node* bind_node::_root = NULL;
sem_mediator * bind_node::_model = NULL;
QHash<int, bind_node*> bind_node::_cache = QHash<int, bind_node*>();

QMap<QString, QString> bind_node::s_oResults = QMap<QString, QString>();
QMap<QString, QString> bind_node::s_oVars = QMap<QString, QString>();

bind_node::bind_node()
{
}

bind_node::~bind_node()
{
	foreach (bind_node *l_o, _children)
	{
		delete l_o;
	}
}

int bind_node::child_count()
{
	return _children.count();
}

bind_node* bind_node::child_num(int i_i)
{
	return _children.at(i_i);
}

void bind_node::set_var(const QString& i_s, const QString &i_s1)
{
	s_oVars[i_s] = i_s1;
}

QString bind_node::get_var(const QString &i_s)
{
	if (i_s == notr("template_dir"))
		return QString(TEMPLATE_DIR);
	if (i_s == notr("filter_dir"))
		return QString(FILTER_DIR);

	//qDebug()<<s_oVars;
	if (!s_oVars.contains(i_s))
	{
		qDebug()<<QString("bindings error: missing variable %1").arg(i_s);
		return "";
	}

	return s_oVars[i_s];
}


void bind_node::set_val(const QString& i_s, const QString &i_s1)
{
	qDebug()<<"not implemented";
}

QString bind_node::get_val(const QString & i_s)
{
	if (i_s == notr("id"))
	{
		return QString::number(m_oItem->m_iId);
	}
	else if (i_s == notr("summary"))
	{
		return m_oItem->m_sSummary;
	}
	else if (i_s == notr("text"))
	{
		return m_oItem->m_sText;
	}
	else if (i_s == notr("hints"))
	{
		return m_oItem->m_sHints;
	}
	else if (i_s == notr("comment"))
	{
		return m_oItem->m_sComment;
	}
	else if (i_s == notr("pic_location"))
	{
		return m_oItem->m_sPicLocation;
	}
	else if (i_s == notr("tree_size"))
	{
		return QString::number(m_oItem->m_oMediator->size_of(m_oItem->m_iId));
	}
	else if (i_s == notr("pic_w"))
	{
		return QString::number(m_oItem->getPix().width());
	}
	else if (i_s == notr("pic_h"))
	{
		return QString::number(m_oItem->getPix().height());
	}
	else if (i_s == notr("type"))
	{
		return QString::number(m_oItem->m_iDataType);
	}
	else if (i_s == notr("pic_id"))
	{
		return QString::number(m_oItem->m_iPicId);
	}
	return "";
}

int bind_node::num_rows()
{
	return m_oItem->m_iNumRows;
}

int bind_node::num_cols()
{
	return m_oItem->m_iNumCols;
}

QString bind_node::tbl_cell(int row, int col)
{
	QPair<int, int> l_o;
	foreach(l_o, m_oItem->m_oTableData.keys()) {
		if (l_o.first == row && l_o.second == col) return m_oItem->m_oTableData[l_o];
	}
	return "";
}

bind_node* bind_node::instance()
{
	return _root;
}

#if 0
QString bind_node::protectCode(const QString &i_s)
{
        QString l_sRet = i_s;
        l_sRet.replace("\n", "\\n");
        l_sRet.replace("\"", "\\\"");
        return l_sRet;
}
#endif

QString bind_node::protectXML(const QString& i_s)
{
	QString l_s = i_s;

	l_s.replace("&", "&amp;");
	l_s.replace("<", "&lt;");
	l_s.replace(">", "&gt;");
	l_s.replace("\'", "&apos;");
	l_s.replace("\"", "&quot;");

	return l_s;
}

QString bind_node::protectHTML(const QString &i_s)
{
	if (i_s.length()<3) return "";

        html_converter l_oHandler;
        QXmlInputSource l_oSource;
        l_oSource.setData(i_s);
        QXmlSimpleReader l_oReader;
        l_oReader.setContentHandler(&l_oHandler);

        if (!l_oReader.parse(l_oSource)) return QString();
	return l_oHandler.m_oTotale.join("");
}

QString bind_node::get_item_ids()
{
	Q_ASSERT(_model != NULL);
	QStringList lst;
	foreach (int id, _model->m_oItems.keys()) {
		lst << QString::number(id);
	}
	return lst.join(",");
}

bind_node* bind_node::get_item_by_id(int id)
{
	Q_ASSERT(_model != NULL);
	bind_node *l_oNode = _cache.value(id);
	if (l_oNode) return l_oNode;
	l_oNode = _cache[id] = new bind_node();
	l_oNode->m_oItem = _model->m_oItems.value(id);
	return l_oNode;
}


bind_node* create_tree(sem_mediator *model, int i_i)
{
	Q_ASSERT(i_i!=0);
	bind_node * l_oNode = new bind_node();
	l_oNode->m_oItem = model->m_oItems.value(i_i);

        for (int i=0; i < model->m_oLinks.size(); i++)
        {
                QPoint l_oP = model->m_oLinks.at(i);
                if (l_oP.x() != i_i) continue;

		bind_node *l_oNew = create_tree(model, l_oP.y());
		l_oNode->_children.push_back(l_oNew);
        }
	return l_oNode;
}

void bind_node::init(sem_mediator* med)
{
	foreach (bind_node * node, _cache.values()) {
		delete node;
	}
	delete bind_node::_root;
	bind_node::_model = med;
	bind_node::_root = create_tree(_model, _model->choose_root());
	bind_node::_cache.clear();
	bind_node::s_oVars.clear();
}

void bind_node::set_result(const QString& k, const QString& v)
{
	s_oResults[k] = v;
}
