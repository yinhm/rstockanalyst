#include "StdAfx.h"
#include "KeyWizard.h"
#include "MainWindow.h"

CKeyWizard* CKeyWizard::m_pSelf = NULL;

CKeyWizard* CKeyWizard::getKeyWizard()
{
	if(m_pSelf==NULL)
		m_pSelf = new CKeyWizard();

	return m_pSelf;
}

CKeyWizard::CKeyWizard( QWidget* parent /*= 0*/ )
	: QWidget(parent)
	, m_pCurWidget(NULL)
{
	setWindowFlags(windowFlags()|Qt::WindowStaysOnTopHint|Qt::Tool);
	setFixedSize(150,200);

	QVBoxLayout* pLayout = new QVBoxLayout();
	pLayout->setContentsMargins(0,0,0,0);
	pLayout->setMargin(0);
	pLayout->setSpacing(0);
	pLayout->addWidget(&m_editInput);
	pLayout->addWidget(&m_viewList);
	setLayout(pLayout);

	connect(&m_editInput,SIGNAL(textChanged(const QString&)),this,SLOT(inputTextChanged(const QString&)));

	CMainWindow* pMain = CMainWindow::getMainWindow();
	if(pMain)
	{
		QPoint ptBR = pMain->mapToGlobal(pMain->childrenRect().bottomRight());
		this->move(ptBR.x()-162,ptBR.y()-230);
	}
}

CKeyWizard::~CKeyWizard(void)
{
}

bool CKeyWizard::showWizard( CBaseWidget* pWidget, const QString& text )
{
	m_pCurWidget = pWidget;
	if(!isVisible())
	{
		m_editInput.setText(text);
	}
	this->show();
	if(::GetForegroundWindow() != winId())
		::SetForegroundWindow(winId());
	return true;
}

void CKeyWizard::hideEvent( QHideEvent* )
{
	m_editInput.clear();
	clearWizData();
}

void CKeyWizard::keyPressEvent( QKeyEvent* e )
{
	if(e->key() == Qt::Key_Escape)
		return hide();
	else if(e->key() == Qt::Key_Enter || e->key()==Qt::Key_Return)
		return enterPressed();

	return QWidget::keyPressEvent(e);
}

void CKeyWizard::inputTextChanged( const QString& text )
{
	if((text.isEmpty())||(!m_pCurWidget))
		return;

	clearWizData();
	m_pCurWidget->getKeyWizData(text,m_listWizData);
	foreach(KeyWizData* pData,m_listWizData)
	{
		QListWidgetItem* pItem = new QListWidgetItem(pData->desc);
		pItem->setData(Qt::UserRole,(uint)pData);
		m_viewList.addItem(pItem);
	}
}

void CKeyWizard::clearWizData()
{
	m_viewList.clear();
	foreach(KeyWizData* p,m_listWizData)
		delete p;
	m_listWizData.clear();
}

void CKeyWizard::enterPressed()
{
	if(!m_pCurWidget)
		return;

	QList<QListWidgetItem*> list = m_viewList.selectedItems();
	if(list.size()>0)
	{
		KeyWizData* pData = reinterpret_cast<KeyWizData*>(list[0]->data(Qt::UserRole).toUInt());
		if(pData)
			m_pCurWidget->keyWizEntered(pData);
	}

	return hide();
}
