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
}

CMainWindow::~CMainWindow()
{

}

bool CMainWindow::setupStockDrv()
{
	if(CSTKDRV::Stock_Init(this->winId(),RSTOCK_ANALYST_MAINMSG,RCV_WORK_SENDMSG)>0)
	{
		if(CSTKDRV::SetupReceiver(TRUE)>0)
			return true;
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
			if(pHeader->m_nPacketNum<1)
				break;

			RCV_REPORT_STRUCTExV3* pReport = pHeader->m_pReportV3;

			int iIndex = 0;
			while(iIndex<pHeader->m_nPacketNum)
			{
				if(pReport->m_cbSize<1)
					break;

				qRcvReportItem* pItem = CDataEngine::getDataEngine()->getBaseMarket(QString::fromLocal8Bit(pReport->m_szLabel));
				if(pItem==NULL)
					pItem = new qRcvReportItem(pReport);
				else
					pItem->resetItem(pReport);

				CDataEngine::getDataEngine()->setBaseMarket(pItem);

				pReport = (pReport+pReport->m_cbSize);
				++iIndex;
			}
			m_pBaseMarketWidget->updateBaseMarket();
		}
		break;

	case RCV_FILEDATA:                      // 共享数据引用方式,文件
		{
			switch(pHeader->m_wDataType)
			{
			case FILE_HISTORY_EX:               // 补日线数据
				break;

			case FILE_MINUTE_EX:                // 补分钟线数据
				break;

			case FILE_BASE_EX:                  // 钱龙兼容基本资料文件,m_szFileName仅包含文件名
				break;

			case FILE_NEWS_EX:                  // 新闻类,其类型由m_szFileName中子目录名来定
				break;
			}
		}
		break;

	default:
		return 0;                           // unknown data
	}
	return 1;
}
