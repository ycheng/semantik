// Thomas Nagy 2007-2012 GPLV3

#include <QPainter>
#include  <QToolTip>
#include <QtDebug>
#include	<QHelpEvent>
#include <QLabel>
#include <QMenu>
#include <QList>
#include <QUrl>
#include <QFileDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QCompleter>
#include <QDirModel>
#include <QGridLayout>
#include <QCoreApplication>

#include "con.h"
#include "data_item.h"
#include "sem_model.h"
#include "image_view.h"

// FIXME change the pictures

image_view::image_view(QWidget *i_oParent, sem_model *i_oControl) : QWidget(i_oParent)
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
		m_oPixmap = l_oData->m_oPix;
	} else {
		m_oPixmap = QPixmap();
		m_iId = NO_ITEM;
	}
	repaint();
}

void image_view::clear_pic()
{
	m_oMediator->m_oImgs.removeAll(m_iId);
	m_oPixmap = QPixmap();

	data_item *l_oData = m_oMediator->m_oItems.value(m_iId);

	l_oData->m_oPix = QPixmap();
	l_oData->m_oThumbnail = QPixmap();

	QString l_s = QString(m_oMediator->m_sTempDir+"/pic-%1.png").arg(QString::number(m_iId));
	if (QFile::exists(l_s))
	{
		QFile l_oFile(l_s);
		l_oFile.remove();
	}
	m_oMediator->notify_change_data(m_iId);
	repaint();
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
	QString l_sText = QFileDialog::getOpenFileName(this, trUtf8("Choose a file"), QString(),
		trUtf8("Image files (*.png *.jpg *.jpeg *.gif)"));
	do_change_pic(l_sText);
}

void image_view::do_change_pic(const QString& l_sText)
{
	if (!QFile::exists(l_sText))
	{
		emit sig_message(trUtf8("File %1 does not exist").arg(l_sText), 20000);
		return;
	}

	QFile l_oInFile(l_sText);
	if (!l_oInFile.open(QIODevice::ReadOnly))
	{
		emit sig_message(trUtf8("File %1 could not be opened").arg(l_sText), 20000);
		return;
	}

	data_item *l_oData = m_oMediator->m_oItems.value(m_iId);
	bool l_bRet = l_oData->load_from_path(l_sText);
	if (!l_bRet)
	{
		emit sig_message(trUtf8("something bad happened"), 20000);
		return;
	}
	m_oMediator->notify_change_data(m_iId);

	m_oPixmap = l_oData->m_oPix;
	repaint();
	//m_oPixmap->setScaledContents(true);

	QStringList l = l_sText.split(".");
	QString l_sNameOut = QString(m_oMediator->m_sTempDir+"/pic-%1.%2").arg(QString::number(m_iId)).arg(l[l.size()-1]);
	QFile l_oOutFile(l_sNameOut);
	if (!l_oOutFile.open(QIODevice::WriteOnly))
	{
		emit sig_message(trUtf8("something worse happened"), 20000);
		return;
	}

	char l_oBuf[8192];
	while (1)
	{
		int l_oLen = l_oInFile.read(l_oBuf, 8192);
		if (l_oLen == 0) break;
		l_oOutFile.write(l_oBuf, l_oLen);
	}
	l_oInFile.close();
	l_oOutFile.close();

	l_oData->m_sPicLocation = l_sText;
	if (!m_oMediator->m_oImgs.contains(m_iId)) m_oMediator->m_oImgs.push_back(m_iId);
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

