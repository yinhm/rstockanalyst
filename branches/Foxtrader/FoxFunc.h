

#ifndef __FOXFUNC_H_INCLUDE
#define __FOXFUNC_H_INCLUDE

/*

飞狐交易师“C语言接口”扩展程序调用接口规范V3.0
1.本规范适用于飞狐交易师V3.x公式系统.
2.扩展函数用于实现系统函数不能实现的特殊算法.
3.扩展函数用windows 32位动态连接库实现,建议使用Microsoft Visual C++编程.
4.调用时在公式编辑器中写"动态库名称@函数名称"(参数表)即可,例如下面函数可以写为"FOXFUNC@MYMACLOSE"(5)
5.动态连接库名称和函数名称可以自己定义.
6.使用时必须将动态库拷贝到飞狐交易师安装目录下的FmlDLL子目录下使用.
*/

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus

// 分析周期
enum DATA_TYPE
{
        TICK_DATA=2,                                //分笔成交
        MIN1_DATA,                                        //1分钟线
        MIN5_DATA,                                        //5分钟线                                        
        MIN15_DATA,                                        //15分钟线
        MIN30_DATA,                                        //30分钟线
        MIN60_DATA,                                        //60分钟线
        DAY_DATA,                                        //日线
        WEEK_DATA,                                        //周线
        MONTH_DATA,                                        //月线
        YEAR_DATA,                                        //年线
        MULTIDAY_DATA,                                //多日线
        MULTIMIN_DATA                                //多分钟线
};


// 基本数据

typedef struct tagSTKDATA        
{
        time_t        m_time;                        //时间,UCT
        float        m_fOpen;                //开盘
        float        m_fHigh;                //最高
        float        m_fLow;                        //最低
        float        m_fClose;                //收盘
        float        m_fVolume;                //成交量
        float        m_fAmount;                //成交额
        WORD        m_wAdvance;                //上涨家数        仅大盘有效
        WORD        m_wDecline;                //下跌家数        仅大盘有效
} STKDATA;

// 扩展数据,用于描述分笔成交数据的买卖盘

typedef union tagSTKDATAEx
{
        struct
        {
                float m_fBuyPrice[3];                //买1--买3价
                float m_fBuyVol[3];                        //买1--买3量
                float m_fSellPrice[3];                //卖1--卖3价        
                float m_fSellVol[3];                //卖1--卖3量 
        };
        float m_fDataEx[12];
} STKDATAEx;

// 除权数据

typedef struct tagSPLITDATA        
{
        time_t        m_time;                        //时间,UCT
        float        m_fHg;                        //红股
        float        m_fPg;                        //配股
        float        m_fPgj;                        //配股价
        float        m_fHl;                        //红利
} SPLITDATA;


/*财务数据顺序(m_pfFinData内容)

        序号        内容

        0        总股本(万股),
        1        国家股,
        2        发起人法人股,
        3        法人股,
        4        B股,
        5        H股,
        6        流通A股,
        7        职工股,
        8        A2转配股,
        9        总资产(千元),
        10        流动资产,
        11        固定资产,
        12        无形资产,
        13        长期投资,
        14        流动负债,
        15        长期负债,
        16        资本公积金,
        17        每股公积金,
        18        股东权益,
        19        主营收入,
        20        主营利润,
        21        其它利润,
        22        营业利润,
        23        投资收益,
        24        补贴收入,
        25        营业外收支,
        26        上年损益调整,
        27        利润总额,
        28        税后利润,
        29        净利润,
        30        未分配利润,
        31        每股未分配,
        32        每股收益,
        33        每股净资产,
        34        调整每股净资,
        35        股东权益比,
        36        净资收益率
*/

// 调用参数项结构
typedef struct tagCALCPARAM
{
        union 
        {
                const float*        m_pfParam;                                //序列参数，指向一个浮点型数组        
                const float                m_fParam;                                //数值参数        
        };
        const int                        m_nParamStart;                        //序列参数有效起始位置
}CALCPARAM;


// 调用接口信息数据结构
typedef struct tagCALCINFO
{
        const DWORD                        m_dwSize;                                //结构大小
        const DWORD                        m_dwVersion;                        //调用软件版本(V2.10 : 0x210)
        const DWORD                        m_dwSerial;                                //调用软件序列号
        const char*                        m_strStkLabel;                        //股票代码
        const BOOL                        m_bIndex;                                //大盘

        const int                        m_nNumData;                                //数据数量(pData,pDataEx,pResultBuf数据数量)
        const STKDATA*                m_pData;                                //常规数据,注意:当m_nNumData==0时可能为 NULL
        const STKDATAEx*        m_pDataEx;                                //扩展数据,分笔成交买卖盘,注意:可能为 NULL

        const int                        m_nParam1Start;                        //参数1有效起始位置
        const float*                m_pfParam1;                                //调用参数1        
        const float*                m_pfParam2;                                //调用参数2
        const float*                m_pfParam3;                                //调用参数3
        const float*                m_pfParam4;                                //调用参数3

        float*                                m_pResultBuf;                        //结果缓冲区
        const DWORD                        m_dataType;                                //数据类型
        const float*                m_pfFinData;                        //财务数据

// 以上与分析家兼容，所以沿用其结构和名称

// 以下为飞狐交易师扩展

        const DWORD                        m_dwReserved;                        // 保留

        const int                        m_nNumParam;                        // 调用参数数量
        const CALCPARAM*        m_pCalcParam;                        // 调用参数数组

        const DWORD                        m_dwReservedEx[4];                // 保留

        char*                                m_strStkName;                        //股票名称

        SPLITDATA*                        m_pSplitData;                        //除权数据
        int                                        m_nNumSplitData;                //除权次数

} CALCINFO;

/* 
注1: (与分析家兼容)
        1.函数调用参数由m_pfParam1--m_pfParam4带入,若为NULL则表示该参数无效.
        2.当一个参数无效时,则其后的所有参数均无效.
                如:m_pfParam2为NULL,则m_pfParam3,m_pfParam4一定为NULL.
        3.参数1可以是常数参数或序列数参数,其余参数只能为常数参数.
        4.若m_nParam1Start<0, 则参数1为常数参数,参数等于*m_pfParam1;
        5.若m_nParam1Start>=0,则参数1为序列数参数,m_pfParam1指向一个浮点型数组,
                数组大小为m_nNumData,数据有效范围为 m_nParam1Start 至 m_nNumData-1.
                在时间上m_pData[x] 与 m_pfParam1[x]是一致的

注2: (飞狐交易师扩展)
        1.该扩展结构使调用参数在技术上可以是无限数目的，且每个参数都可为数值或序列，由公式中实际的调用参数决定。
        2.CALCPARAM结构用于带入参数信息和实际数据，m_pCalcParam数组大小为m_nNumParam，数据有效范围为 0 至 m_nNumParam-1.
        3.按参数的顺序，m_pCalcParam[0]为第一个参数的数据，m_pCalcParam[1]为第二个参数的数据...，为了保持兼容，原m_nParam1Start、m_pfParam1等5个属性依然有赋值。
        4.若 i位置的参数为数值，取用m_pCalcParam[i].m_fParam.
        5.若 i位置的参数为序列，取用m_pCalcParam[i].m_pfParam，数组大小为m_nNumData,数据有效范围为m_pCalcParam[i].m_nParamStart 至 m_nNumData-1. 若m_pCalcParam[i].m_nParamStart<0, 则此数组中无有效数据。
        6.由于可以调用多个序列，许多序列的计算可以先在公式中进行，然后作为调用的参数即可。
        7.经此扩展，对分析家的DLL依然可以调用、兼容。

*/

/* 函数输出

__declspec(dllexport) int xxxxxxxx(CALCINFO* pData);

1.函数名称需全部大写.
2.函数必须以上述形式声明,请用实际函数名称替代xxxxxxxx;
        对于C++程序还需包括在 extern "C" {   } 括号中.
3.函数计算结果用pData->m_pResultBuf带回.
4.函数返回-1表示错误或全部数据无效,否则返回第一个有效值位置,即:
        m_pResultBuf[返回值] -- m_pResultBuf[m_nNumData-1]间为有效值.
5.函数名称长度不能超过15字节,动态连接库文件名不能超过9字节(不包括扩展名),动态库名称不能叫SYSTEM,EXPLORER

*/

//示例函数,使用时用实际名称替换

// __declspec(dllexport) int WINAPI MYMACLOSE(CALCINFO* pData);
__declspec(dllexport) int WINAPI SMOOTH(CALCINFO* pData);
// __declspec(dllexport) int WINAPI MYMAVAR(CALCINFO* pData);
// __declspec(dllexport) int WINAPI MYBBI(CALCINFO* pData);

#ifdef __cplusplus
}
#endif //__cplusplus


#endif //__FOXFUNC_H_INCLUDE

