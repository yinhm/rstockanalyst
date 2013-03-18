// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but

#if !defined(AFX_STDAFX_H__61A31CEB_9707_11D1_ACAB_0000E823DBFD__INCLUDED_)
#define AFX_STDAFX_H__61A31CEB_9707_11D1_ACAB_0000E823DBFD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#define _USE_32BIT_TIME_T			//ÉèÖÃ time_t Îª4×Ö½Ú

#include <limits>
#include <Windows.h>

#include "StockDrv.h"

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
};

#define	FLOAT_NAN	(std::numeric_limits<float>::quiet_NaN())

#endif // !defined(AFX_STDAFX_H__61A31CEB_9707_11D1_ACAB_0000E823DBFD__INCLUDED_)
