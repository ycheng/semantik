// Thomas Nagy 2007-2012 GPLV3

#ifndef DATA_CONTROL_H
#define DATA_CONTROL_H

#include <QObject>
#include <QStack>
#include <QXmlDefaultHandler>
#include<KUrl> 
#include <kio/netaccess.h>

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


class semantik_reader;
class data_item;
class bind_node;
class sem_mediator: public QObject
{
	Q_OBJECT

	signals:
		//void rectangle_changed(int i);

		void sig_message(const QString&, int);
		void dirty(bool);
		void update_title();

		void sig_add_item(int id);
		void sig_delete_item(int id);
		void sig_link_items(int id1, int id2);
		void sig_unlink_items(int id1, int id2);
		void sync_flags();
		void sync_colors();
		void sig_select(const QList<int>& unsel, const QList<int>& sel);
		void sig_move(const QList<int>&sel, const QList<QPointF>&pos);
		void sig_repaint(int id);
		void sig_flag(int id);
		void sig_edit(int id);
		void sig_text(int id);
		void sig_datatype(int id);
		void sig_preview();
		void sig_vars(int id);
		void sig_pic(int id);
		void sig_table(int id);
		void sig_sort(int id);
		void sig_open_map();
		void sig_change_data(int id);
		void sig_export_item(int id);
		void sig_focus(void*);

		void sig_add_box(int, int);
		void sig_del_box(int, int);
		void sig_edit_box(int, int);
		void sig_link_box(int, data_link*);
		void sig_unlink_box(int, data_link*);
		void sig_box_props(int id, const QList<diagram_item*>&);
		void sig_pos_box(int id, const QList<data_box*>&);
		void sig_change_link_box(int id, data_link*);
		void sig_size_box(int id, const QList<data_box*>&);

		void enable_undo(bool, bool);

	public:
		sem_mediator(QObject *i_oParent);
		~sem_mediator();

		void undo_purge();

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

		void init_colors();
		void init_flags();

		// first parameter is the parent, second parameter is the wanted id
		// and the third parameter is for copying the parents data
		bool link_items(int id1, int id2);

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

		QStack<mem_command*> m_oUndoStack;
		QStack<mem_command*> m_oRedoStack;

		void notify_add_item(int id);
		void notify_delete_item(int id);
		void notify_link_items(int id1, int id2);
		void notify_unlink_items(int id1, int id2);
		void notify_select(const QList<int>& unsel, const QList<int>& sel);
		void notify_move(const QList<int>&sel, const QList<QPointF>&pos);
		void notify_repaint(int id);
		void notify_edit(int id);
		void notify_text(int id);
		void notify_vars(int id);
		void notify_flag(int id);
		void notify_pic(int id);
		void notify_datatype(int id);
		void notify_table(int id);
		void notify_sort(int id);
		void notify_change_data(int id);
		void notify_export_item(int id);
		void notify_focus(void *);

		void notify_add_box(int id, int box);
		void notify_del_box(int id, int box);
		void notify_edit_box(int id, int box);
		void notify_link_box(int id, data_link *link);
		void notify_unlink_box(int id, data_link *link);
		void notify_box_props(int id, const QList<diagram_item*>&);
		void notify_pos_box(int id, const QList<data_box*>&);
		void notify_change_link_box(int id, data_link*);
		void notify_size_box(int id, const QList<data_box*>&);

		void notify_message(const QString& msg, int duration);

	public:

		KUrl m_oCurrentUrl;

		QHash<int, data_item*> m_oItems;
		QList<QPoint> m_oLinks;
		QList<int> m_oImgs;

		QList<color_scheme> m_oColorSchemes;
		QList<flag_scheme*> m_oFlagSchemes;

		int next_seq();
		int next_pic_seq();

		QPair<int, int> hint_size_diagram(int);
		QHash<int, data_pic*> m_oPixCache;


		QPixmap getPix(int id);
		QPixmap getThumb(int id);
		bool load_picture(const QString&, int);
		bool save_and_load_picture(const KUrl& i_sPath, int id);


		void generate_docs(const QString &i_oFile, const QString &i_sName, const QString &i_sLocation);

		int m_iTimerValue;
		int parent_of(int i_iId);
		int size_of(int i_iId);
		int choose_root();

		void init_timer();
		void destroy_timer();

		QString doc_to_xml();
		QString m_sLastSaved;

		data_item* operator+(const int x);

		void select_item_keyboard(int, int);
		void private_select_item(int i_oId);

		void check_undo(bool); // check if the undo/redo actions can be enabled
		int itemSelected();

		friend class semantik_reader;

	private:
		int num_seq;
		int pic_seq;
		QTimer *m_oTimer;

	public slots:
		//void do_reorganize();
		void slot_autosave();
		void slot_undo();
		void slot_redo();
};

QIcon fetch_icon(const QString& i_sName, int i_iSize=32);

#endif

