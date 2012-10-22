#include "StdAfx.h"
#include "MainWindow.h"
#include "STKDRV.h"
#include "BaseMarketWidget.h"
#include "DataEngine.h"
#include <iostream>
using namespace std;

#define RSTOCK_ANALYST_MAINMSG (WM_USER+1)

CMainWindow::CMainWindow()
	: QMainWindow()
{
	m_pMdiArea = new QMdiArea();
	m_pMdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	m_pMdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	setCentralWidget(m_pMdiArea);

	m_pBaseMarketWidget = new CBaseMarketWidget;

	m_pMdiArea->addSubWindow(m_pBaseMarketWidget);

	//Init F10 data( base info)
	QSettings settings("HKEY_LOCAL_MACHINE\\SOFTWARE\\StockDrv",QSettings::NativeFormat);
	QString qsF10File = settings.value("SSInfoPath").toString() + "\\财务数据.fin";
	if(QFile::exists(qsF10File))
	{
		CDataEngine::importBaseInfoFromFile(qsF10File);
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
			cout<< "####Comming reports####"<<endl;
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

				qRcvReportData* p = new qRcvReportData(pReport);
				pItem->appendReport(p);
			}
		}
		break;

	case RCV_FILEDATA:                      // 共享数据引用方式,文件
		{
			switch(pHeader->m_wDataType)
			{
			case FILE_HISTORY_EX:               // 补日线数据
				{
					if(pHeader->m_nPacketNum<1)
						break;

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

					cout<<"Packet cout:"<<pHeader->m_nPacketNum<<endl;
					cout<<"UseTime:"<<QTime::currentTime().msecsTo(timeBegin)<<"m secs"<<endl;
				}
				break;

			case FILE_MINUTE_EX:                // 补分钟线数据
				{
					int i = 0;
				}
				break;

			case FILE_BASE_EX:                  // 钱龙兼容基本资料文件,m_szFileName仅包含文件名
				{
					int i = 0;
				}
				break;

			case FILE_NEWS_EX:                  // 新闻类,其类型由m_szFileName中子目录名来定
				{
					int i = 0;
				}
				break;
			case FILE_POWER_EX:
				{
					int i = 0;
				}
				break;
			default:
				{
					int j = 0;
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
