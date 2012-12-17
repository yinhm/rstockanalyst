// STKDRV.cpp: implementation of the CSTKDRV class.
//tel:13366898744
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "STKDRV.h"
#include <QtCore>

int (WINAPI* CSTKDRV::m_pfnStock_Init)(HWND hWnd,UINT Msg,int nWorkMode)= NULL;
int (WINAPI* CSTKDRV::m_pfnStock_Quit)(HWND hWnd)= NULL;
int (WINAPI* CSTKDRV::m_pfnGetTotalNumber)();
int (WINAPI* CSTKDRV::m_pfnGetStockByNoEx)(int nNo,RCV_REPORT_STRUCTExV3* pBuf)= NULL;
int (WINAPI* CSTKDRV::m_pfnGetStockByCodeEx)(char* pszStockCode,int nMarket,RCV_REPORT_STRUCTExV3* pBuf)= NULL;
int	(WINAPI* CSTKDRV::m_pfnSetupReceiver)(BOOL bSetup)= NULL;
DWORD (WINAPI* CSTKDRV::m_pfnGetStockDrvInfo)(int nInfo,void * pBuf)= NULL;
void (WINAPI* CSTKDRV::m_pfnReInitStockInfo)() = NULL;


HINSTANCE CSTKDRV::m_hSTKDrv = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSTKDRV::CSTKDRV()
{

}

CSTKDRV::~CSTKDRV()
{
	if(m_hSTKDrv)
	{
		FreeLibrary(m_hSTKDrv);
		m_hSTKDrv = NULL;
	}
}

void CSTKDRV::InitStockDrv()
{
	QSettings settings("HKEY_LOCAL_MACHINE\\SOFTWARE\\StockDrv",QSettings::NativeFormat);
	QString qsDriver = settings.value("driver").toString();
	if(QFile::exists(qsDriver))
	{
		m_hSTKDrv = LoadLibrary(qsDriver.toStdWString().data());
	//	m_hSTKDrv = LoadLibrary(L"C:\\YjStock\\Stockdrv.dll");
	}
	else
	{
		m_hSTKDrv = LoadLibrary(L"Stock.dll");
	}


	if( !m_hSTKDrv ) 
		LoadLibrary(L"Stock.dll");
	if( !m_hSTKDrv ) 
		return;

	m_pfnStock_Init = \
		(int (WINAPI*)(HWND,UINT,int)) GetProcAddress(m_hSTKDrv,"Stock_Init");
	m_pfnStock_Quit = \
		(int (WINAPI*)(HWND)) GetProcAddress(m_hSTKDrv,"Stock_Quit");
	m_pfnGetTotalNumber = \
		(int (WINAPI*)())GetProcAddress(m_hSTKDrv,"GetTotalNumber");
	m_pfnGetStockByNoEx = \
		(int (WINAPI*)(int,RCV_REPORT_STRUCTExV3*))GetProcAddress(m_hSTKDrv,"GetStockByNoEx");
	m_pfnGetStockByCodeEx = \
		(int (WINAPI*)(char*,int,RCV_REPORT_STRUCTExV3*))GetProcAddress(m_hSTKDrv,"GetStockByCodeEx");
	m_pfnSetupReceiver = \
		(int	(WINAPI *)(BOOL))GetProcAddress(m_hSTKDrv,"SetupReceiver");
	m_pfnGetStockDrvInfo = \
		(DWORD (WINAPI *)(int,void * ))GetProcAddress(m_hSTKDrv,"GetStockDrvInfo");
	m_pfnReInitStockInfo = \
		(void (WINAPI *)())GetProcAddress(m_hSTKDrv,"ReInitStockInfo");
}

int CSTKDRV::Stock_Init(HWND hWnd, UINT uMsg, int nWorkMode)
{
	if(!m_pfnStock_Init)
		return -1;
	return ((*m_pfnStock_Init)(hWnd,uMsg,nWorkMode));
}

int CSTKDRV::Stock_Quit(HWND hWnd)
{
	if(!m_pfnStock_Quit)
		return -1;
	return ((*m_pfnStock_Quit)(hWnd));
}

int CSTKDRV::GetTotalNumber()
{
	if( !m_pfnGetTotalNumber )
		return -1;
	return ((*m_pfnGetTotalNumber)());
}

int CSTKDRV::GetStockByNoEx( int nNo,RCV_REPORT_STRUCTExV3* pBuf )
{
	if(!m_pfnGetStockByNoEx)
		return -1;
	return((*m_pfnGetStockByNoEx)(nNo,pBuf));
}

int CSTKDRV::GetStockByCodeEx( char* pszStockCode,int nMarket,RCV_REPORT_STRUCTExV3* pBuf )
{
	if(!m_pfnGetStockByCodeEx)
		return -1;
	return ((*m_pfnGetStockByCodeEx)(pszStockCode,nMarket,pBuf));
}

int CSTKDRV::SetupReceiver(BOOL bSetup)
{
	if(!m_pfnSetupReceiver)
		return -1;
	return ((*m_pfnSetupReceiver)(bSetup));
}

DWORD CSTKDRV::GetStockDrvInfo(int nInfo, void* pBuf)
{
	if(!m_pfnGetStockDrvInfo)
		return -1;
	return ((*m_pfnGetStockDrvInfo)(nInfo,pBuf));
}

void CSTKDRV::ReInitStockInfo()
{
	if(!m_pfnReInitStockInfo)
		return;
	return ((*m_pfnReInitStockInfo)());
}
