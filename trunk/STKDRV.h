// STKDRV.h: interface for the CSTKDRV class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STKDRV_H__4BE51F0E_A261_11D2_B30C_00C04FCCA334__INCLUDED_)
#define AFX_STKDRV_H__4BE51F0E_A261_11D2_B30C_00C04FCCA334__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include <QtCore>

class CSTKDRV  
{
public:
	CSTKDRV();
	virtual ~CSTKDRV();
public:
	static void InitStockDrv();
	static int Stock_Init(HWND hWnd,UINT uMsg,int nWorkMode);
	static int Stock_Quit(HWND hWnd);
	static int GetTotalNumber();
	static int GetStockByNoEx(int nNo,RCV_REPORT_STRUCTEx* pBuf);
	static int GetStockByCodeEx(char* pszStockCode,int nMarket,RCV_REPORT_STRUCTEx* pBuf);
	static int SetupReceiver(BOOL bSetup);
	static DWORD GetStockDrvInfo(int nInfo,void* pBuf);



private:
	static int (WINAPI* m_pfnStock_Init)(HWND hWnd,UINT Msg,int nWorkMode);
	static int (WINAPI* m_pfnStock_Quit)(HWND hWnd);
	static int (WINAPI* m_pfnGetTotalNumber)();
	static int (WINAPI* m_pfnGetStockByNoEx)(int nNo,RCV_REPORT_STRUCTEx* pBuf);
	static int (WINAPI* m_pfnGetStockByCodeEx)(char* pszStockCode,int nMarket,RCV_REPORT_STRUCTEx* pBuf);
	static int	(WINAPI* m_pfnSetupReceiver)(BOOL bSetup);
	static DWORD (WINAPI* m_pfnGetStockDrvInfo)(int nInfo,void * pBuf);

	static HINSTANCE	m_hSTKDrv;
};

#endif 
