#include "StdAfx.h"
#include "MainWindow.h"
#include "STKDRV.h"
#include "BaseMarketWidget.h"
#include "DataEngine.h"


#define RSTOCK_ANALYST_MAINMSG (WM_USER+1)

CMainWindow::CMainWindow()
	: QMainWindow()
{
	{
		//初始化布局
		m_pMdiArea = new QMdiArea();
		m_pMdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
		m_pMdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
		m_pMdiArea->setViewMode(QMdiArea::TabbedView);
		setCentralWidget(m_pMdiArea);

		//添加多版面窗口
		m_pTemplateWidget = new QTabWidget(this);
		m_pSubTemplate = new CRMdiSubWindow(m_pMdiArea);
		m_pSubTemplate->setWindowTitle(tr("个性化版面"));
		m_pSubTemplate->setWidget(m_pTemplateWidget);

		//添加基本行情窗口
		m_pBaseMarketWidget = new CBaseMarketWidget;
		m_pSubBaseMarket = new CRMdiSubWindow(m_pMdiArea);
		m_pSubBaseMarket->setWindowTitle(tr("基本行情"));
		m_pSubBaseMarket->setWidget(m_pBaseMarketWidget);
		m_pSubBaseMarket->showMaximized();
	//	m_pMdiArea->setActiveSubWindow(m_pSubBaseMarket);
	}

	{
		//初始化Menu
		m_pMenuBar = new QMenuBar(this);
		//基本行情
		QMenu* pMenuBaseMarket = m_pMenuBar->addMenu(tr("基本行情"));
		//版面管理
		QMenu* pMenuTemplate = m_pMenuBar->addMenu(tr("版面管理"));
		pMenuTemplate->addAction(tr("添加版面"),this,SLOT(onAddTemplate()));
		setMenuBar(m_pMenuBar);

		//视图菜单，包含各SubWindow的显示与否
		QMenu* pMenuView = m_pMenuBar->addMenu(tr("视图"));
		pMenuView->addAction(tr("基本行情"),this,SLOT(onActiveBaseMarket()));
		pMenuView->addAction(tr("版面管理"),this,SLOT(onActiveTemplate()));
	}

}

CMainWindow::~CMainWindow()
{

}

bool CMainWindow::setupStockDrv()
{
	if(CSTKDRV::Stock_Init(this->winId(),RSTOCK_ANALYST_MAINMSG,RCV_WORK_MEMSHARE)>0)
	{
		if(CSTKDRV::SetupReceiver(TRUE)>0)
		{
			return true;
		}
	}

	return false;
}

bool CMainWindow::winEvent( MSG* message, long* result )
{
	if(message->message == RSTOCK_ANALYST_MAINMSG)
	{
		*result = OnStockDrvMsg(message->wParam,message->lParam);
	}

	return QMainWindow::winEvent(message,result);
}

long CMainWindow::OnStockDrvMsg( WPARAM wParam,LPARAM lParam )
{
	RCV_DATA* pHeader;
	pHeader = (RCV_DATA*)lParam;

	//  对于处理速度慢的数据类型,最好将 pHeader->m_pData 指向的数据备份,再作处理

	switch( wParam )
	{
	case RCV_REPORT:                        // 共享数据引用方式,股票行情
		{
			qDebug()<< "####Comming reports"<<QTime::currentTime().toString()<<"####";
			if(pHeader->m_nPacketNum<1)
				break;

			RCV_REPORT_STRUCTExV3* pReport = pHeader->m_pReportV3;
			for(int i=0;i<pHeader->m_nPacketNum;++i)
			{
				pReport = (pHeader->m_pReportV3+i);

				QString qsCode = QString::fromLocal8Bit(pReport->m_szLabel);

				CStockInfoItem* pItem = CDataEngine::getDataEngine()->getStockInfoItem(qsCode);
				if(pItem==NULL)
				{
					pItem = new CStockInfoItem(qsCode,pReport->m_wMarket);
					CDataEngine::getDataEngine()->setStockInfoItem(pItem);
				}

				pItem->setReport(pReport);
			}
		}
		break;
	case RCV_FILEDATA:                      // 共享数据引用方式,文件
		{
			switch(pHeader->m_wDataType)
			{
			case FILE_HISTORY_EX:               // 补日线数据
				{
					QTime timeBegin = QTime::currentTime();
					RCV_HISTORY_STRUCTEx* pHistory = pHeader->m_pDay;
					QString qsCode;

					QList<qRcvHistoryData*> listHistory;
 					for(int i=0;i<pHeader->m_nPacketNum;++i)
					{
						pHistory = (pHeader->m_pDay+i);
						if(pHistory->m_time == EKE_HEAD_TAG)
						{
							CStockInfoItem* pItem = CDataEngine::getDataEngine()->getStockInfoItem(qsCode);
							if(pItem==NULL)
							{
								//删除指针，防止内存泄漏
								foreach(qRcvHistoryData* p,listHistory)
									delete p;
							}
							else
							{
								pItem->appendHistorys(listHistory);
							}
							qsCode = QString::fromLocal8Bit(pHistory->m_head.m_szLabel);
							listHistory.clear();
						}
						else
						{
							listHistory.append(new qRcvHistoryData(pHistory));
						}
					}

					qDebug()<<"Packet cout:"<<pHeader->m_nPacketNum;
					qDebug()<<"UseTime:"<<QTime::currentTime().msecsTo(timeBegin)<<"m secs";
				}
				break;

			case FILE_MINUTE_EX:                // 补分钟线数据
				{
					qDebug()<<"Minute Packet cout:"<<pHeader->m_nPacketNum;

					RCV_MINUTE_STRUCTEx* pMinute = pHeader->m_pMinute;
					QString qsCode;

					QList<qRcvMinuteData*> listMinute;
					for(int i=0;i<pHeader->m_nPacketNum;++i)
					{
						pMinute = (pHeader->m_pMinute+i);

						if(pMinute->m_time == EKE_HEAD_TAG)
						{
							CStockInfoItem* pItem = CDataEngine::getDataEngine()->getStockInfoItem(qsCode);
							if(pItem==NULL)
							{
								//删除指针，防止内存泄漏
								foreach(qRcvMinuteData* p,listMinute)
									delete p;
							}
							else
							{
								pItem->appendMinutes(listMinute);
							}
							qsCode = QString::fromLocal8Bit(pMinute->m_head.m_szLabel);
							listMinute.clear();
						}
						else
						{
							listMinute.append(new qRcvMinuteData(pMinute));
						}
					}
				}
				break;

			case FILE_BASE_EX:                  // 钱龙兼容基本资料文件,m_szFileName仅包含文件名
				{
					int i = 0;
				}
				break;

			case FILE_NEWS_EX:                  // 新闻类,其类型由m_szFileName中子目录名来定
				{
					QString qsNewsTitle = QString::fromLocal8Bit(pHeader->m_File.m_szFileName);
					qDebug()<<"Comming News "<< qsNewsTitle;
					QString qsNewsPath = qApp->applicationDirPath() + "\\news\\" + qsNewsTitle;

					QDir().mkpath(QFileInfo(qsNewsPath).absolutePath());
					QFile file(qsNewsPath);
					if(!file.open(QFile::WriteOnly|QFile::Truncate))
						break;
					file.write((char*)pHeader->m_pData,pHeader->m_File.m_dwLen);
					file.close();
				}
				break;
			case FILE_POWER_EX:
				{
					qDebug()<<"Power Packet cout:"<<pHeader->m_nPacketNum;

					RCV_POWER_STRUCTEx* pPower = pHeader->m_pPower;
					QString qsCode;

					QList<qRcvPowerData*> listPower;
					for(int i=0;i<pHeader->m_nPacketNum;++i)
					{
						pPower = (pHeader->m_pPower+i);

						if(pPower->m_time == EKE_HEAD_TAG)
						{
							CStockInfoItem* pItem = CDataEngine::getDataEngine()->getStockInfoItem(qsCode);
							if(pItem==NULL)
							{
								//删除指针，防止内存泄漏
								foreach(qRcvPowerData* p,listPower)
									delete p;
							}
							else
							{
								pItem->appendPowers(listPower);
							}
							qsCode = QString::fromLocal8Bit(pPower->m_head.m_szLabel);
							listPower.clear();
						}
						else
						{
							listPower.append(new qRcvPowerData(pPower));
						}
					}
				}
				break;
			default:
				{
					int i = 0;
				}
				break;
			}
		}
		break;

	default:
		return 0;                           // unknown data
	}
	return 1;
}

void CMainWindow::onActiveBaseMarket()
{
	m_pSubBaseMarket->showMaximized();
}

void CMainWindow::onActiveTemplate()
{
	m_pSubTemplate->showMaximized();
}

void CMainWindow::onAddTemplate()
{

}
