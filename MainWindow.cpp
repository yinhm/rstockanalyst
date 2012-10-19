#include "StdAfx.h"
#include "MainWindow.h"
#include "STKDRV.h"
#include <iostream>
using namespace std;

#define RSTOCK_ANALYST_MAINMSG (WM_USER+1)

CMainWindow::CMainWindow( QWidget* parent/*=0*/, Qt::WindowFlags flags/*=0 */ )
	: QMainWindow(parent,flags)
{

}

CMainWindow::~CMainWindow(void)
{

}

bool CMainWindow::setupStockDrv()
{
	if(CSTKDRV::Stock_Init(this->winId(),RSTOCK_ANALYST_MAINMSG,RCV_WORK_MEMSHARE)>0)
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
	int i;

	pHeader = (RCV_DATA*)lParam;

	//  对于处理速度慢的数据类型,最好将 pHeader->m_pData 指向的数据备份,再作处理

	switch( wParam )
	{
	case RCV_REPORT:                        // 共享数据引用方式,股票行情
		cout<<"文件类型:"<<pHeader->m_wDataType<<endl;
		for(i=0; i<pHeader->m_nPacketNum; i++)
		{
			cout<<pHeader->m_pReport[i].m_szName<<endl;
//			RCV_REPORT_STRUCTEx pBuffer = pHeader->m_pReport[i];
//			cout<<pBuffer.m_szName<<endl;
		}
		break;

	case RCV_FILEDATA:                      // 共享数据引用方式,文件
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
		break;

	default:
		return 0;                           // unknown data
	}
	return 1;
}
