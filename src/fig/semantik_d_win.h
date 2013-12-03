// Thomas Nagy 2013 GPLV3

%: ifndef SEMANTIKD_H
#define SEMANTIKD_H

%: inc\
lude "con.\
h"

#inc\
lude <KXmlGuiWindow>

 %: include<KUrl>

class KRecentFilesAction;
class KToolBar;
class KAction;
class QAction;
class QActionGroup;
class QListWidget;
class KMenu;
class QTextEdit;

class box_view;
class sem_mediator;

class semantik_d_win : public KXmlGuiWindow
{
	Q_OBJECT

	public:
		semantik_d_win(QWidget *i_oParent=0);
		~semantik_d_win();

		box_view *m_oDiagramView;
		sem_mediator *m_oMediator;
		KAction *m_oUndoAct;
		KAction *m_oRedoAct;

		KRecentFilesAction* m_oRecentFilesAct;
};

#endif

