#include "StdAfx.h"
#include "FlashWidget.h"


CFlashWidget::CFlashWidget( CBaseWidget* parent /*= 0*/ )
	: CBaseWidget(parent,WidgetFlash)
	, m_iSwitchSeconds(10)
	, m_iTitleHeight(20)
{
	m_pMenuCustom = new QMenu(tr("快速切换图菜单"));
	m_pMenuAddWidget = m_pMenuCustom->addMenu(tr("增加窗口"));
	foreach(const RWidgetOpData& _d,m_listWidget)
	{
		if(_d.value==WidgetFlash)
			continue;

		m_pMenuAddWidget->addAction(_d.desc,this,SLOT(onAddWidget()))->setData(_d.value);
	}

	m_timerSwitch.start(m_iSwitchSeconds*1000);
	connect(&m_timerSwitch,SIGNAL(timeout()),this,SLOT(onSwitchNextWidget()));
}


CFlashWidget::~CFlashWidget(void)
{
	foreach(CBaseWidget* pWidget,m_listWidgetsStack)
	{
		delete pWidget;
	}
	m_listWidgetsStack.clear();
}

bool CFlashWidget::loadPanelInfo( const QDomElement& eleWidget )
{
	if(!CBaseWidget::loadPanelInfo(eleWidget))
		return false;

	//add here
	
//	setLayout(&m_layout);
	m_pLayout->setContentsMargins(QMargins(0,m_iTitleHeight,0,0));
	return true;
}

bool CFlashWidget::savePanelInfo( QDomDocument& doc,QDomElement& eleWidget )
{
	if(!CBaseWidget::savePanelInfo(doc,eleWidget))
		return false;

	//add here

	return true;
}

QMenu* CFlashWidget::getCustomMenu()
{
	QAction* pAction = m_pMenu->menuAction();
	if(!m_pMenuCustom->actionGeometry(pAction).isValid())
		m_pMenuCustom->addMenu(m_pMenu);

	return m_pMenuCustom;
}

void CFlashWidget::paintEvent( QPaintEvent* e )
{

	return CBaseWidget::paintEvent(e);
}

void CFlashWidget::onAddWidget()
{
	QAction* pAct = reinterpret_cast<QAction*>(sender());

	//获取并重置窗口类型
	CBaseWidget* pWidget = createBaseWidget(this,static_cast<RWidgetType>(pAct->data().toInt()));
	m_listWidgetsStack.append(pWidget);

	m_pSplitter->addWidget(pWidget);
	restNewWidget(pWidget);
}

void CFlashWidget::onSwitchNextWidget()
{
	qDebug()<<m_listWidgetsStack.size();

	int iIndex = m_pSplitter->indexOf(m_pCurWidget);
	++iIndex;
	if(iIndex>=m_pSplitter->count())
		iIndex = 0;
	QWidget* pWidget = m_pSplitter->widget(iIndex);
	if(pWidget)
		restNewWidget(pWidget);
}

void CFlashWidget::restNewWidget( QWidget* pWidget )
{
	if(m_pSplitter->count()<1)
		return;

	int iIndex = m_pSplitter->indexOf(pWidget);
	if(iIndex<0)
		return;
	QList<int> listSizes;
	for (int i=0;i<m_pSplitter->count();++i)
	{
		if(i == iIndex)
			listSizes.append(100);
		else
			listSizes.append(0);
	}
	m_pSplitter->setSizes(listSizes);
	m_pCurWidget = reinterpret_cast<CBaseWidget*>(pWidget);
}
