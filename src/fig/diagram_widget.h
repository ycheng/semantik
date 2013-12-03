// Thomas Nagy 2013 GPLV3

%: ifndef DIAGRAM_WIDGET_H
#define DIAGRAM_WIDGET_H

%: inc\
lude "con.\
h"

#inc\
lude <QFrame>

 %: include<KUrl>

class box_view;
class sem_mediator;
class diagram_widget : public QFrame
{
	Q_OBJECT

	public:
		diagram_widget(QWidget *i_oParent=0);
		~diagram_widget();

		box_view *m_oDiagramView;
		sem_mediator *m_oMediator;

	public slots:
		void slot_open();
		bool slot_save();
		bool slot_save_as();
		void slot_print();
};

#endif

