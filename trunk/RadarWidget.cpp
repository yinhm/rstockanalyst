#include "StdAfx.h"
#include "RadarWidget.h"
#include "DataEngine.h"
#include "MainWindow.h"
#include "WatcherSettingDlg.h"
#define TIMER_AUTO_SCROLL	30*1000

void CRadarWidget::testRandomRadar()
{
	static int i = 1;
	QList<CStockInfoItem*> listStocks = CDataEngine::getDataEngine()->getStockInfoList();
	CStockInfoItem* pItem = listStocks[qrand()%listStocks.size()];

	RRadarData* pRadar = new RRadarData;
	pRadar->pStock = pItem;
	pRadar->tmTime = QDateTime::currentDateTime().toTime_t();
	pRadar->tpType = BigVolumn;
	pRadar->qsDesc = QString("Index:%1").arg(i);
	pRadar->iWatcher = 2;
	CRadarManager::getRadarManager()->appendRadar(pRadar);
	++i;
}

CRadarWidget::CRadarWidget( CBaseWidget* parent /*= 0*/ )
	: CBaseWidget(parent, WidgetRadar)
	, m_iItemHeight(16)
	, m_iTitleHeight(16)
	, m_pSelRadar(0)
	, m_iShowIndex(0)
{
	m_pMenuCustom = new QMenu(tr("雷达监视菜单"));
	m_pMenuCustom->addAction(tr("过滤器设置"),this,SLOT(onSetFilter()));

	connect(CRadarManager::getRadarManager(),SIGNAL(radarAlert(RRadarData*)),this,SLOT(onRadarAlert(RRadarData*)));

	//连接超时自动滚动到顶部的定时器
	connect(&m_timerAutoScroll,SIGNAL(timeout()),this,SLOT(onAutoScroll()));

	/*临时用于生成监视数据*/
	QTimer* pTimer = new QTimer();
	connect(pTimer,SIGNAL(timeout()),this,SLOT(testRandomRadar()));
	pTimer->start(1000);
}


CRadarWidget::~CRadarWidget(void)
{
}

bool CRadarWidget::loadPanelInfo( const QDomElement& eleWidget )
{
	if(!CBaseWidget::loadPanelInfo(eleWidget))
		return false;

	//当前的过滤器
	QDomElement eleFilters = eleWidget.firstChildElement("Filters");
	if(eleFilters.isElement())
	{
		QDomElement eleFilter = eleFilters.firstChildElement("Filter");
		while(eleFilter.isElement())
		{
			int iFilter = eleFilter.text().toInt();
			if(CRadarManager::getRadarManager()->getWatcher(iFilter))
			{
				m_mapFilter[iFilter] = iFilter;
			}

			eleFilter = eleFilter.nextSiblingElement("Filter");
		}
	}

	return true;
}

bool CRadarWidget::savePanelInfo( QDomDocument& doc,QDomElement& eleWidget )
{
	if(!CBaseWidget::savePanelInfo(doc,eleWidget))
		return false;

	//当前的表达式
	QDomElement eleFilters = doc.createElement("Filters");
	QMap<int,int>::iterator iter = m_mapFilter.begin();
	while(iter!=m_mapFilter.end())
	{
		QDomElement eleFilter = doc.createElement("Filter");
		eleFilter.appendChild(doc.createTextNode(QString("%1").arg(iter.key())));

		eleFilters.appendChild(eleFilter);
		++iter;
	}
	eleWidget.appendChild(eleFilters);

	return true;
}

void CRadarWidget::getKeyWizData( const QString& keyword,QList<KeyWizData*>& listRet )
{

	return CBaseWidget::getKeyWizData(keyword,listRet);
}

void CRadarWidget::keyWizEntered( KeyWizData* pData )
{

	return CBaseWidget::keyWizEntered(pData);
}


void CRadarWidget::paintEvent( QPaintEvent* /*e*/ )
{
	QPainter p(this);
	QRect rtClient = this->rect();
	p.fillRect(rtClient,QColor(0,0,0));

	m_rtTitle = QRect(0,0,rtClient.width(),m_iTitleHeight);
	m_rtClient = QRect(0,m_iTitleHeight,rtClient.width(),rtClient.height()-m_iTitleHeight);

	drawTitle(p);
	drawClient(p);
}

void CRadarWidget::keyPressEvent( QKeyEvent* e )
{
	m_timerAutoScroll.start(TIMER_AUTO_SCROLL);
	int iKey = e->key();
	if(Qt::Key_Down == iKey)
	{
		if(m_pSelRadar)
		{
			int iCurIndex = m_mapRadarsIndex[m_pSelRadar];
			if(iCurIndex>0)
			{
				RRadarData* pRadar = m_listRadars[iCurIndex-1];
				clickedRadar(pRadar);
			}
		}
		e->accept();
	}
	else if(Qt::Key_Up == iKey)
	{
		if(m_pSelRadar)
		{
			int iCurIndex = m_mapRadarsIndex[m_pSelRadar];
			if(m_listRadars.size()>(iCurIndex+1))
			{
				RRadarData* pRadar = m_listRadars[iCurIndex+1];
				clickedRadar(pRadar);
			}
		}
		e->accept();
	}
	else if(Qt::Key_PageDown == iKey)
	{
		int iShowCount = m_rtClient.height()/m_iItemHeight;
		if(iShowCount<1)
			return;
		if((m_iShowIndex+iShowCount)<m_listRadars.size())
		{
			m_iShowIndex = m_iShowIndex+iShowCount;
			update(m_rtClient);
		}
		e->accept();
	}
	else if(Qt::Key_PageUp == iKey)
	{
		int iShowCount = m_rtClient.height()/m_iItemHeight;
		if(iShowCount<1)
			return;
		m_iShowIndex = (m_iShowIndex-iShowCount)>0 ? (m_iShowIndex-iShowCount) : 0;
		update(m_rtClient);

		e->accept();
	}
	else if(Qt::Key_F10 == iKey)
	{
		//F10数据
		if(m_pSelRadar)
		{
			if(!CDataEngine::getDataEngine()->showF10Data(m_pSelRadar->pStock->getCode()))
			{
				//未打开F10数据 do something
			}
		}
	}
	else
	{
		return CBaseWidget::keyPressEvent(e);
	}
}

void CRadarWidget::mouseMoveEvent( QMouseEvent* e )
{
	QPoint ptCur = e->pos();
	if(m_rtClient.contains(ptCur))
	{
		RRadarData* pData = testRadarData(ptCur);
		if(pData)
		{
			//do something;
		}
	}
	e->accept();
	return CBaseWidget::mouseMoveEvent(e);
}

void CRadarWidget::mousePressEvent( QMouseEvent* e )
{
	QPoint ptCur = e->pos();
	if(m_rtClient.contains(ptCur))
	{
		RRadarData* pData = testRadarData(ptCur);
		if(pData)
		{
			clickedRadar(pData);
		}
	}
}

void CRadarWidget::wheelEvent( QWheelEvent* e )
{
	int numDegrees = e->delta() / 8;
	int numSteps = numDegrees / 15;
	int iIndex = m_iShowIndex-numSteps*5;
	if(iIndex<0) {iIndex = 0;}
	if(iIndex>=0&&iIndex<m_listRadars.size())
	{
		m_timerAutoScroll.start(TIMER_AUTO_SCROLL);
		e->accept();
		m_iShowIndex = iIndex;
		update(m_rtClient);
	}
	return CBaseWidget::wheelEvent(e);
}

QMenu* CRadarWidget::getCustomMenu()
{
	QAction* pAction = m_pMenu->menuAction();
	if(!m_pMenuCustom->actionGeometry(pAction).isValid())
		m_pMenuCustom->addMenu(m_pMenu);

	return m_pMenuCustom;
}

void CRadarWidget::onRadarAlert( RRadarData* pRadar )
{
	if(!m_mapFilter.contains(pRadar->iWatcher))
		return;

	m_mapRadarsIndex[pRadar] = m_listRadars.size();
	m_listRadars.append(pRadar);
	if(m_iShowIndex>0)
	{
		++m_iShowIndex;
	}
	else
	{
		update(m_rtClient);
	}
}

void CRadarWidget::onAutoScroll()
{
	m_timerAutoScroll.stop();
	m_iShowIndex = 0;
	update(m_rtClient);
}

void CRadarWidget::onSetFilter()
{
	CWatcherSettingDlg dlg(m_mapFilter.keys(),this);
	dlg.exec();
	QList<int> listIDs = dlg.getSelIDs();
	m_mapFilter.clear();

	foreach(const int& i,listIDs)
	{
		m_mapFilter[i] = i;
	}
}

void CRadarWidget::drawTitle( QPainter& p )
{
	QRect rtTitle = m_rtTitle;
	if(!rtTitle.isValid())
		return;

	QRect rtClient = rtTitle.adjusted(2,0,-2,0);

	p.setPen(QColor(127,0,0));
	p.drawRect(rtClient);

	p.setPen(QColor(255,255,255));
	p.drawText(rtClient,Qt::AlignLeft|Qt::AlignVCenter,"雷达监视");
	p.setPen(QColor(233,0,0));
	p.drawRect(m_rtTitle);
}

void CRadarWidget::drawClient( QPainter& p )
{
	int iCount = m_rtClient.height()/m_iItemHeight + 1;
	if(m_listRadars.size()<iCount)
		iCount = m_listRadars.size();

	int iBeginY = m_rtClient.top();
	int iSize = m_listRadars.size()-m_iShowIndex-1;
	p.setPen(QColor(255,255,255));
	for(int i=0;i<iCount;++i)
	{
		if(iSize-i>=0&&iSize-i<m_listRadars.size())
		{
			RRadarData* pData = m_listRadars[iSize-i];
			QRect rtItem(0,iBeginY+m_iItemHeight*i,m_rtClient.width(),m_iItemHeight);

			if(pData == m_pSelRadar)
			{
				p.fillRect(rtItem,QColor(244,0,0));
			}

			QString qsShowText = QString("%1 %2").arg(pData->pStock->getCode(),pData->qsDesc);
			p.drawText(rtItem,qsShowText);
		}
	}
}

void CRadarWidget::clickedRadar( RRadarData* pRadar )
{
	if(!pRadar)
		return;
	{
		//设置显示的起始位置
		int iRow = m_mapRadarsIndex[pRadar];

		int iShowCount = m_rtClient.height()/m_iItemHeight;
		if(iShowCount<1)
			return;
		qDebug()<<"last show index:"<<m_iShowIndex;
		int iSize = m_listRadars.size();

		if((iSize-m_iShowIndex-iRow)>=iShowCount)
		{
			m_iShowIndex = iSize-iRow-iShowCount;
		}
		else if(m_iShowIndex+iRow >= iSize)
		{
			m_iShowIndex = iSize-iRow-1;
		}
	}

	m_pSelRadar = pRadar;
	update();
	if(pRadar->pStock)
	{
		CMainWindow::getMainWindow()->clickedStock(pRadar->pStock->getOnly());
	}
}

RRadarData* CRadarWidget::testRadarData( const QPoint& ptClicked )
{
	if(!m_rtClient.contains(ptClicked))
		return 0;
	int iIndex = (ptClicked.y()-m_rtClient.top()) / m_iItemHeight;
	if(iIndex<0)
		return 0;
	iIndex = m_listRadars.size() - m_iShowIndex - iIndex - 1;
	if(iIndex >= m_listRadars.size() || iIndex<0)
		return 0;

	return m_listRadars[iIndex];
}
