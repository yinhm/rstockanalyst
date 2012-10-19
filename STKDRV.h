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
	int Stock_Init(HWND hWnd,UINT uMsg,int nWorkMode);
	int Stock_Quit(HWND hWnd);
	int GetTotalNumber();
	int GetStockByNoEx(int nNo,RCV_REPORT_STRUCTEx* pBuf);
	int GetStockByCodeEx(char* pszStockCode,int nMarket,RCV_REPORT_STRUCTEx* pBuf);
	int SetupReceiver(BOOL bSetup);
	DWORD GetStockDrvInfo(int nInfo,void* pBuf);



private:
	int (WINAPI* m_pfnStock_Init)(HWND hWnd,UINT Msg,int nWorkMode);
	int (WINAPI* m_pfnStock_Quit)(HWND hWnd);
	int (WINAPI* m_pfnGetTotalNumber)();
	int (WINAPI* m_pfnGetStockByNoEx)(int nNo,RCV_REPORT_STRUCTEx* pBuf);
	int (WINAPI* m_pfnGetStockByCodeEx)(char* pszStockCode,int nMarket,RCV_REPORT_STRUCTEx* pBuf);
	int	(WINAPI* m_pfnSetupReceiver)(BOOL bSetup);
	DWORD (WINAPI* m_pfnGetStockDrvInfo)(int nInfo,void * pBuf);

private:
	void GetAdress();
	HINSTANCE	m_hSTKDrv;
};

#endif 
