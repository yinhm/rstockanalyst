#include "StdAfx.h"
#include "SplashDlg.h"
#include <windows.h>

CSplashDlg::CSplashDlg( QWidget* parent /*= 0*/ )
	: QWidget(parent,Qt::SplashScreen|Qt::WindowStaysOnTopHint)
{
	m_pProgressBar = new QProgressBar(this);
	m_pProgressBar->setMaximum(100);
	m_pProgressBar->setValue(1);
	m_pProgressBar->setTextVisible(true);

	m_pLabel = new QLabel(this);

	QVBoxLayout* pMainLayout = new QVBoxLayout(this);

	this->setLayout(pMainLayout);
	pMainLayout->addWidget(m_pLabel);
	pMainLayout->addWidget(m_pProgressBar);
}

CSplashDlg::~CSplashDlg()
{
	delete m_pProgressBar;
}

void CSplashDlg::showMessage( const QString& msg, int iPro /*= 90*/ )
{
	m_pLabel->setText(msg);
	m_pProgressBar->setValue(iPro);
	qApp->processEvents();
	Sleep(500);
}

void CSplashDlg::showEvent(QShowEvent* event)
{
	QDesktopWidget* desktop = QApplication::desktop();
	move((desktop->width() - this->width())/2, (desktop->height() - this->height())/2);

	return QWidget::showEvent(event);
}
