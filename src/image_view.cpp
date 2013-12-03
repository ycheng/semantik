// Thomas Nagy 2007-2012 GPLV3

#include <QPainter>
#include  <QToolTip>
#include <QtDebug>
#include	<QHelpEvent>
#include <QLabel>
#include <QMenu>
#include <QList>
#include <QFile>
#include   <QFileInfo>
#include <QUrl>
#include <QFileDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QCompleter>
#include <QDirModel>
#include <QGridLayout>
#include <QCoreApplication>
#include  <KFileDialog>

#include "con.h"
#include "data_item.h"
#include "sem_mediator.h"
#include "image_view.h"
#include "mem_command.h"

image_view::image_view(QWidget *i_oParent, sem_mediator *i_oControl) : QWidget(i_oParent)
{
	setCursor(Qt::PointingHandCursor);
	m_oMediator = i_oControl;
	m_iId = NO_ITEM;

	setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(context_menu(const QPoint&)));

	m_oChangePictureAction = new QAction(trUtf8("Change picture"), this);
        connect(m_oChangePictureAction, SIGNAL(triggered()), this, SLOT(change_pic()));
	addAction(m_oChangePictureAction);

	m_oClearPictureAction = new QAction(trUtf8("Clear picture"), this);
        connect(m_oClearPictureAction, SIGNAL(triggered()), this, SLOT(clear_pic()));
	addAction(m_oClearPictureAction);

	setAcceptDrops(true);

	m_oMenu = NULL;

	/*
	QPalette l_oPalette = m_oSelect->palette();
	l_oPalette.setBrush(QPalette::Inactive, QPalette::Base, QColor(230, 230, 230));
	l_oPalette.setBrush(QPalette::Disabled, QPalette::Base, QColor(230, 230, 230));
	m_oSelect->setPalette(l_oPalette);
	*/
}

void image_view::paintEvent(QPaintEvent*)
{
	QPainter p(this);
	QRect r = rect();
	if (!r.width() or !r.height()) return;
	if (m_oPixmap.isNull())
	{
		if (m_iId)
		{
			p.drawText(r, Qt::AlignCenter, trUtf8("Click to set a picture"));
		}
		else
		{
			p.drawText(r, Qt::AlignCenter, trUtf8("Select an item on the map"));
		}
		return;
	}

	QRect rp = m_oPixmap.rect();

	if (rp.width() < r.width() && rp.height() < r.height())
	{
		QRect ru((r.width() - rp.width())/2, (r.height() - rp.height())/2, rp.width(), rp.height());
		p.drawPixmap(ru, m_oPixmap);
	}
	else if (rp.width() * r.height() / rp.height() - r.width() > 0)
	{
		int a = r.width();
		int b = rp.height() * r.width() / rp.width();

		QRect ru( 0, qAbs(r.height() - b) / 2, a, b);
		p.drawPixmap(ru, m_oPixmap);
	}
	else
	{
		int a = rp.width() * r.height() / rp.height();
		int b = r.height();

		QRect ru( qAbs(r.width() - a) / 2, 0, a, b);
		p.drawPixmap(ru, m_oPixmap);
	}
}

void image_view::notify_select(const QList<int>& unsel, const QList<int>& sel) {
	bool one = (sel.size() == 1);
	if (one) {
		m_iId = sel.at(0);
		data_item *l_oData = m_oMediator->m_oItems.value(m_iId);
		m_oPixmap = l_oData->getPix();
	} else {
		m_oPixmap = QPixmap();
		m_iId = NO_ITEM;
	}
	repaint();
}

void image_view::clear_pic()
{
	data_item *l_oData = m_oMediator->m_oItems.value(m_iId);

	mem_pic *mem = new mem_pic(m_oMediator);
	mem->sel = l_oData;
	mem->m_iOldId = l_oData->m_iPicId;
	mem->m_iNewId = NO_ITEM;
	mem->apply();
}

#include "image_view.moc"

bool image_view::event(QEvent *i_oEvent)
{
        if (i_oEvent->type() == QEvent::ToolTip)
        {
		if (m_iId)
		{
			QHelpEvent *l_oEv = static_cast<QHelpEvent*>(i_oEvent);
			data_item *l_oData = m_oMediator->m_oItems.value(m_iId);
			QToolTip::showText(l_oEv->globalPos(), l_oData->m_sPicLocation);
		}
        }
	return QWidget::event(i_oEvent);
}

void image_view::context_menu(const QPoint& i_o)
{
	if (!m_iId) return;
	if (!m_oMenu)
	{
		m_oMenu = new QMenu(this);
		m_oMenu->addAction(m_oChangePictureAction);
		m_oMenu->addAction(m_oClearPictureAction);
	}
	m_oMenu->popup(mapToGlobal(i_o));
}

void image_view::mouseReleaseEvent(QMouseEvent *i_o)
{
	if (!m_iId || !rect().contains(i_o->pos()) || i_o->button() != Qt::LeftButton)
	{
		i_o->ignore();
		return;
	}
	i_o->accept();
	change_pic();
}

void image_view::change_pic()
{
	KUrl l_o = KFileDialog::getOpenUrl(KUrl(notr("kfiledialog:///image")),
                trUtf8("*.png *.jpg *.jpeg *.gif|Image Files (*.png *.jpg *.jpeg *.gif)"),
                this, trUtf8("Choose a picture"));
	do_change_pic(l_o);
}

void image_view::do_change_pic(const KUrl& l_sText)
{
	data_item *l_oData = m_oMediator->m_oItems.value(m_iId);
	int id = m_oMediator->next_pic_seq();

	bool l_bRet = m_oMediator->save_and_load_picture(l_sText, id);
	if (!l_bRet)
	{
		emit sig_message(trUtf8("Could not load the picture"), 20000);
		return;
	}

	mem_pic *mem = new mem_pic(m_oMediator);
	mem->sel = l_oData;
	mem->m_iOldId = l_oData->m_iPicId;
	mem->m_iNewId = id;
	mem->apply();
}

void image_view::dragEnterEvent(QDragEnterEvent *i_o)
{
	if (!m_iId) return;

	const QMimeData *l_oMimeData = i_o->mimeData();
	if (l_oMimeData->hasText() || l_oMimeData->hasUrls()) // TODO handle image data || l_oMimeData->hasImage())
	{
		i_o->acceptProposedAction();
	}
}

void image_view::dragMoveEvent(QDragMoveEvent *i_o)
{
	if (!m_iId) return;
	i_o->acceptProposedAction();
}

void image_view::dropEvent(QDropEvent *i_o)
{
	if (!m_iId) return;
	const QMimeData *l_oMimeData = i_o->mimeData();
	if (l_oMimeData->hasUrls())
	{
		QList<QUrl> urlList = l_oMimeData->urls();
		for (int i = 0; i < urlList.size() && i < 32; ++i)
		{
			QUrl l_o = urlList.at(i);
			if (!l_o.isValid()) continue;
			do_change_pic(l_o.toLocalFile());
			break;
		}
	}
	else if (l_oMimeData->hasText())
	{
		qDebug()<<"new picture is "<<l_oMimeData->text();
		do_change_pic(l_oMimeData->text());
	}
	//else if (l_oMimeData->hasImage()) TODO handle image data

	i_o->acceptProposedAction();
}

void image_view::dragLeaveEvent(QDragLeaveEvent *i_o)
{
	if (!m_iId) return;
	i_o->accept();
}

void image_view::notify_pic(int id)
{
	if (m_iId == id)
	{
		data_item *l_oData = m_oMediator->m_oItems.value(m_iId);
		m_oPixmap = l_oData->getPix();
		repaint();
	}
}

void image_view::notify_export_item(int id)
{
	data_item *l_oData = m_oMediator->m_oItems.value(id);
	if (l_oData->m_iDataType != VIEW_IMG)
		return;
	if (l_oData->m_iPicId != NO_ITEM)
	{
		QDir l_oDir(m_oMediator->m_sTempDir);
		QFileInfoList l_oLst = l_oDir.entryInfoList();
		foreach (QFileInfo l_oInfo, l_oLst) {
			QString l_sName = l_oInfo.fileName();
			if (l_sName.startsWith(notr("img-%1").arg(QString::number(l_oData->m_iPicId))))
			{
				QFile f(l_oInfo.absoluteFilePath());
				QString newName = l_oInfo.fileName().replace(QRegExp("img-\\d+"), QString("diag-%1").arg(QString::number(id)));
				f.copy(newName); // TODO works for now
				break;
			}
		}
	}
}

