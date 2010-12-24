// Thomas Nagy 2007-2011 GPLV3

#ifndef DATA_CONTROL_H
#define DATA_CONTROL_H

#include <QObject>
#include <QStack>
#include <QXmlDefaultHandler>
#include <QMutex>
#include<KUrl> 

#include "aux.h"
#include "con.h"
#include "mem_command.h"
#include "flag_scheme.h"
#include "color_scheme.h"

class QTimer;
class html_converter : public QXmlDefaultHandler
{
	public:
		html_converter();

		bool startElement(const QString&, const QString&, const QString&, const QXmlAttributes&);
		bool endElement(const QString&, const QString&, const QString&);
		bool characters(const QString &i_sStr);

		QString m_sBuf;
		QStringList m_oTotale;
};

class data_item;
class bind_node;
class sem_model: public QObject
{
	Q_OBJECT

	signals:
		//void rectangle_changed(int i);

		void synchro(const hash_params&);

		void sig_message(const QString&, int);
		void dirty(bool);
		void update_title();

	public:
		sem_model(QObject *i_oParent);
		~sem_model();

		QString m_sTempDir;
		QString m_sOutDir;
		QString m_sOutProject;
		QString m_sOutTemplate;

		void set_dirty(bool b = true);
		bool m_bDirty;

		void init_temp_dir();
		void clean_temp_dir();

		bool save_file(QString);
		bool open_file(const QString&);
		void purge_document();
		bool read_xml_file(const QString &l_oBa);

		void init_colors();
		void init_flags();

		// first parameter is the parent, second parameter is the wanted id
		// and the third parameter is for copying the parents data
		int add_item(int i_oAdd=NO_ITEM, int i_iIdx=NO_ITEM, bool i_iCopy=false);

		void unlink_items(int id1, int id2);
		bool link_items(int id1, int id2);
		void remove_item(int i_oId);
		void dis_connect(int i_oId);
		void select_item(int i_oId, int i_iView=NO_VIEW);
		void update_item(int i_oId, int i_iView=NO_VIEW);

		void sort_children(int i_iParent, int i_iChild, int i_iNum);
		int num_children(int i_iParent);
		int m_iConnType;
		int m_iReorgType;
		void select_root_item(int);
		QList<int> all_roots();
		int root_of(int i_iId);
		void next_root();
		void prev_root();
		double m_dTriSize;

		QColor m_oColor;
		QString m_sHints;

		int m_iLastItemSelected;


		QStack<mem_command> m_oUndoStack;
		QStack<mem_command> m_oRedoStack;

	private:
		int m_sCount;
		QTimer *m_oTimer;
	public:

		KUrl m_oCurrentUrl;

		QHash<int, data_item*> m_oItems;
		QList<QPoint> m_oLinks;
		QList<int> m_oImgs;

		QList<color_scheme> m_oColorSchemes;
		QList<flag_scheme*> m_oFlagSchemes;

		int get_next();

		void generate_docs(const QString &i_oFile, const QString &i_sName, const QString &i_sLocation);

		int m_iTimerValue;
		int parent_of(int i_iId);
		int size_of(int i_iId);
		bind_node* create_tree(int);
		int choose_root();

		void init_timer();
		void destroy_timer();

		QString doc_to_xml();
		QString m_sLastSaved;

		data_item* operator+(const int x);

		void select_item_keyboard(int, int);

		void change_data(int i_iId, int i_iType);
		QMutex m_oLock;

	public slots:
		//void do_reorganize();
		void slot_autosave();
		void slot_undo();
		void slot_redo();
};

QIcon fetch_icon(const QString& i_sName, int i_iSize=32);

#endif

