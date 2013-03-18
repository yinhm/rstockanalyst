

// FoxFunc.cpp : Defines the entry point for the DLL application.
//
#include <string.h>
#include <stdio.h>
#include <io.h>
#include <math.h>
#include <iostream.h>

#include "stdafx.h"
#include "FoxFunc.h"

float y[15000];
float yy[15000];

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
                                         )
{
    switch (ul_reason_for_call)
        {
                case DLL_PROCESS_ATTACH:
                case DLL_THREAD_ATTACH:
                case DLL_THREAD_DETACH:
                case DLL_PROCESS_DETACH:
                        break;
    }
    return TRUE;
}



// FoxFunc.cpp : Defines the entry point for the DLL application.
//
#include <string.h>
#include <stdio.h>
#include <io.h>
#include <math.h>
#include <iostream.h>

#include "stdafx.h"
#include "FoxFunc.h"

float y[15000];
float yy[15000];

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
                                         )
{
    switch (ul_reason_for_call)
        {
                case DLL_PROCESS_ATTACH:
                case DLL_THREAD_ATTACH:
                case DLL_THREAD_DETACH:
                case DLL_PROCESS_DETACH:
                        break;
    }
    return TRUE;
}

//计算收盘价的均价,一个常数参数,表示计算周期
//调用方法:
//        "FOXFUNC@MYMACLOSE"(5)


float smoothb(int ,int );
float realft(int, int );
int fourit(int , int );

//        !!!!!!!! use "C" for Microsoft visual C++ compiler
//        !!!!!!!! use "C" for Microsoft visual C++ compiler
//        !!!!!!!! use "C" for Microsoft visual C++ compiler


__declspec(dllexport) int WINAPI SMOOTH(CALCINFO* pData)
{
        if(pData->m_pfParam1 && pData->m_pfParam2 &&         //参数1,2有效
                pData->m_nParam1Start>=0 &&                                        //参数1为序列数
                pData->m_pfParam3==NULL)                                        //有2个参数
        {
                const float*  pValue = pData->m_pfParam1;        //参数1
                int nFirst = pData->m_nParam1Start;                        //有效值起始位
                float fParam = *pData->m_pfParam2;                        //参数2
                int nPeriod = (int)fParam;                        
        
        int nndata =pData->m_nNumData;
        float aa=(float)0.0;


                if( nFirst >= 0 && nPeriod > 0 )
                {   
                //        float fTotal;
                        int i;

   
                        for ( i =nFirst; i < pData->m_nNumData; i++ )
                        {
                                y[i+1] = pValue[i];    //平均
                        }
      
            for ( i = 0; i <15000; i++ )
                        {
                                yy[i+1]=(float)0.0;
                        }


              aa=smoothb(nndata,nPeriod);


            for ( i =nFirst; i < pData->m_nNumData; i++ )
                        {
                                pData->m_pResultBuf[i]=y[i+1];    //平均
                //   pData->m_pResultBuf[i]=aa;    //平均
            //    pData->m_pResultBuf[i]=nndata;  
                        }

                        return 0;
                }
        }
        return -1;
}

