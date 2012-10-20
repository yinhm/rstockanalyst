/*银江行情接口规范V1.02
****************************************************************************************/
/*	驱动程序安装后,在系统注册库中注册了以下信息,分析软件通过该注册信息启动引擎动态库*
/*	HKEY_LOCAL_MACHINE/SOFTWARE/StockDrv 下						* 
/*	Driver  = "驱动DLL全路径"							*
/****************************************************************************************/

#ifndef __STOCKDRV_H__
#define __STOCKDRV_H__

#pragma   pack(push,1)					//设置内存对齐方式为 1字节

// 工作方式类型定义
#define RCV_WORK_SENDMSG	4

/////////////////////////////////////////////////////////////////////////////////////////

// 证券市场
#define		SH_MARKET_EX			'HS'		// 上海
#define		SZ_MARKET_EX			'ZS'		// 深圳
#define     HK_MARKET_EX            'KH'        //香港市场
#define     EB_MARKET_EX            'BE'        //深圳二板市场

// 文件数据类型
// 结构数组形式的文件数据
#define		FILE_HISTORY_EX			2		// 补日线数据
#define		FILE_MINUTE_EX			4		// 补分钟线数据
#define		FILE_POWER_EX			6		// 补充除权数据
#define		FILE_5MINUTE_EX			81		// 补5分钟线数据

#define		FILE_BASE_EX			0x1000		// 钱龙兼容基本资料文件,m_szFileName仅包含文件名
#define		FILE_NEWS_EX			0x1002		// 新闻类,其类型由m_szFileName中子目录名来定

// 消息子类型
#define		News_Sha_Ex			2		// 上证消息
#define		News_Szn_Ex			4		// 深证消息
#define		News_Fin_Ex			6		// 财经报道
#define		News_Unknown_Ex			-1		// 未知提供者

//Definition For nInfo of Function GetStockDrvInfo(int nInfo,void * pBuf);
#define		RI_IDSTRING			1			// 厂商名称,返回(LPCSTR)厂商名
#define		RI_IDCODE			2			// 卡号
#define		RI_VERSION			3			// 驱动程序版本

#define		RI_NETPCID			1000			//  - 返回电脑硬件编号

#define		STKLABEL_LEN			10			// 股号数据长度,国内市场股号编码兼容钱龙
#define		STKNAME_LEN			32			// 股名长度



//////////////////////////////////////////////////////////////////////////////////
//行情数据
//注:
//		某些字段可能为0,用上次的数据代替
typedef struct tagRCV_REPORT_STRUCTEx
{
	WORD	m_cbSize;									// 结构大小
	time_t	m_time;										// 交易时间
	WORD	m_wMarket;									// 股票市场类型
	char	m_szLabel[STKLABEL_LEN];					// 股票代码,以'\0'结尾
	char	m_szName[STKNAME_LEN];						// 股票名称,以'\0'结尾

	float	m_fLastClose;								// 昨收
	float	m_fOpen;									// 今开
	float	m_fHigh;									// 最高
	float	m_fLow;										// 最低
	float	m_fNewPrice;								// 最新
	float	m_fVolume;									// 成交量,以股为单位,股的含义依股票类型定义
	float	m_fAmount;									// 成交额,以元为单位

	float	m_fBuyPrice[3];								// 申买价1,2,3
	float	m_fBuyVolume[3];							// 申买量1,2,3
	float	m_fSellPrice[3];							// 申卖价1,2,3
	float	m_fSellVolume[3];							// 申卖量1,2,3


	float	m_fBuyPrice4;								// 申买价4
	float	m_fBuyVolume4;								// 申买量4
	float	m_fSellPrice4;								// 申卖价4
	float	m_fSellVolume4;								// 申卖量4

} RCV_REPORT_STRUCTEx;


//////////////////////////////////////////////////////////////////////////////////
//行情数据(版本数据结构RCV_WORK_SENDMSG)
typedef struct tagRCV_REPORT_STRUCTExV2	
{
	WORD	m_cbSize;							// 结构大小
	time_t	m_time;								// 成交时间
	WORD	m_wMarket;							// 股票市场类型
	char	m_szLabel[STKLABEL_LEN];					// 股票代码,以'\0'结尾
	char	m_szName[STKNAME_LEN];						// 股票名称,以'\0'结尾
	
	float	m_fLastClose;							// 昨收
	float	m_fOpen;							// 今开
	float	m_fHigh;							// 最高
	float	m_fLow;								// 最低
	float	m_fNewPrice;							// 最新
	float	m_fVolume;							// 成交量
	float	m_fAmount;							// 成交额
	float	m_fBuyPrice[3];							// 申买价1,2,3
	float	m_fBuyVolume[3];						// 申买量1,2,3
	float	m_fSellPrice[3];						// 申卖价1,2,3
	float	m_fSellVolume[3];						// 申卖量1,2,3

	float	m_fBuyPrice4;							// 申买价4
	float	m_fBuyVolume4;							// 申买量4
	float	m_fSellPrice4;							// 申卖价4
	float	m_fSellVolume4;							// 申卖量4
} RCV_REPORT_STRUCTExV2;

//////////////////////////////////////////////////////////////////////////////////
//行情数据(版本3数据结构RCV_WORK_SENDMSG，兼容RCV_REPORT_STRUCTExV2结构，使用m_cbSize区分不同结构)
typedef struct tagRCV_REPORT_STRUCTExV3	
{
	WORD	m_cbSize;							// 结构大小
	time_t	m_time;								// 成交时间
	WORD	m_wMarket;							// 股票市场类型
	char	m_szLabel[STKLABEL_LEN];					// 股票代码,以'\0'结尾
	char	m_szName[STKNAME_LEN];						// 股票名称,以'\0'结尾
	
	float	m_fLastClose;							// 昨收
	float	m_fOpen;							// 今开
	float	m_fHigh;							// 最高
	float	m_fLow;								// 最低
	float	m_fNewPrice;							// 最新
	float	m_fVolume;							// 成交量
	float	m_fAmount;							// 成交额
	float	m_fBuyPrice[3];							// 申买价1,2,3
	float	m_fBuyVolume[3];						// 申买量1,2,3
	float	m_fSellPrice[3];						// 申卖价1,2,3
	float	m_fSellVolume[3];						// 申卖量1,2,3

	float	m_fBuyPrice4;							// 申买价4
	float	m_fBuyVolume4;							// 申买量4
	float	m_fSellPrice4;							// 申卖价4
	float	m_fSellVolume4;							// 申卖量4

	float	m_fBuyPrice5;							// 申买价5
	float	m_fBuyVolume5;							// 申买量5
	float	m_fSellPrice5;							// 申卖价5
	float	m_fSellVolume5;							// 申卖量5
} RCV_REPORT_STRUCTExV3;

//////////////////////////////////////////////////////////////////////////////////
//补充数据头
//数据头 m_dwHeadTag == EKE_HEAD_TAG 
#define EKE_HEAD_TAG	0xffffffff

typedef struct	tagRCV_EKE_HEADEx
{
	DWORD	m_dwHeadTag;							// = EKE_HEAD_TAG
	WORD	m_wMarket;							// 市场类型
	char	m_szLabel[STKLABEL_LEN];					// 股票代码
}RCV_EKE_HEADEx;

//补充日线数据
typedef union tagRCV_HISTORY_STRUCTEx
{
	struct
	{
		time_t	m_time;				//UCT
		float	m_fOpen;			//开盘
		float	m_fHigh;			//最高
		float	m_fLow;				//最低
		float	m_fClose;			//收盘
		float	m_fVolume;			//量
		float	m_fAmount;			//额
		WORD	m_wAdvance;			//涨数,仅大盘有效
		WORD	m_wDecline;			//跌数,仅大盘有效
	};
	RCV_EKE_HEADEx	m_head;
}RCV_HISTORY_STRUCTEx;

//补充分时线数据
typedef union tagRCV_MINUTE_STRUCTEx
{
	struct{
		time_t	m_time;				// UCT
		float	m_fPrice;
		float	m_fVolume;
		float	m_fAmount;
	};
	RCV_EKE_HEADEx	m_head; 
}RCV_MINUTE_STRUCTEx;

//补充除权数据
typedef union tagRCV_POWER_STRUCTEx
{
	struct
	{
		time_t	m_time;				// UCT
		float	m_fGive;			// 每股送
		float	m_fPei;				// 每股配
		float	m_fPeiPrice;			// 配股价,仅当 m_fPei!=0.0f 时有效
		float	m_fProfit;			// 每股红利
	};
	RCV_EKE_HEADEx	m_head;
}RCV_POWER_STRUCTEx;


//分笔数据///////////////////////////////////
typedef struct tagYJFenbi 
{				
	long	m_lTime;			// hhmmss 例：93056 表明9:30:56的盘口数据

	float	m_fHigh;			// 最高
	float	m_fLow;				// 最低
	float	m_fNewPrice;			// 最新
	float	m_fVolume;			// 成交量
	float	m_fAmount;			// 成交额

	long	m_lStroke;			// 保留
	float	m_fBuyPrice[5];			// 申买价1,2,3,4,5
	float	m_fBuyVolume[5];		// 申买量1,2,3,4,5
	float	m_fSellPrice[5];		// 申卖价1,2,3,4,5
	float	m_fSellVolume[5];		// 申卖量1,2,3,4,5
} RCV_FENBI_STRUCTEx;

struct YJ_Fenbi
{
	WORD	m_wMarket;			// 股票市场类型　　　　　　　　　　 同通视规范定义
	char	m_szLabel[STKLABEL_LEN];	// 股票代码,以'\0'结尾
	long	m_lDate;			// 分笔数据的日期 FORMAT: 20050808,  0表示当天或最近交易日的明细数据
	float	m_fLastClose;			// 昨收
	float	m_fOpen;			// 今开
	WORD	m_nCount;			//m_Data的数据量分笔笔数(2字节smallint)
	RCV_FENBI_STRUCTEx	m_Data[1];	//长度为m_nCount*sizeof(RCV_FENBI_STRUCTEx)字节	
} RCV_FENBI;
/////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
// 文件类型数据包头
//  注一:
//	m_wDataType == FILE_BASE_EX
//				m_dwAttrib = 股票证券市场,m_szFileName仅包含文件名
//	m_wDataType == FILE_NEWS_EX
//				m_dwAttrib = 消息来源,m_szFileName 包含目录的文件名,目录名为消息来源
//				如:  "上交所消息\\SH0001.TXT","会员交流\\000001.TXT"
typedef struct tagRCV_FILE_HEADEx
{
	DWORD	m_dwAttrib;					// 文件子类型
	DWORD	m_dwLen;					// 文件长度
	DWORD	m_dwSerialNo;					// 序列号
	char	m_szFileName[MAX_PATH];				// 文件名 or URL
} RCV_FILE_HEADEx;

//////////////////////////////////////////////////////////////////////////////////
// 数据通知消息
//直接数据引用通知消息
//	wParam = RCV_WPARAM;
//	lParam 指向 RCV_DATA结构;
//	返回 1 已经处理, 0 未处理或不能处理

#define RCV_REPORT		0x3f001234
#define RCV_FILEDATA		0x3f001235
#define RCV_FENBIDATA		0x3f001301


// 注一:
//	  记录数表示行情数据和补充数据(包括 Header)的数据包数,对文件类型数据, = 1
// 注二:
//	  若 m_bDISK = FALSE, m_pData 为数据缓冲区指针
//		 ******** 数据共享,不能修改数据 **********
//		 m_bDISK = TRUE,  m_pData 为该文件的存盘文件名,一般情况只有
//		 升级软件等大文件用存盘方式
typedef struct tagRCV_DATA
{
	int					m_wDataType;		// 文件类型
	int					m_nPacketNum;		// 记录数,参见注一
	RCV_FILE_HEADEx		m_File;					// 文件接口
	BOOL				m_bDISK;			// 文件是否已存盘的文件
	union
	{
		RCV_REPORT_STRUCTExV2*	m_pReportV2;
		RCV_REPORT_STRUCTExV3*	m_pReportV3;
		RCV_HISTORY_STRUCTEx *	m_pDay;
		RCV_MINUTE_STRUCTEx  *	m_pMinute;
		RCV_POWER_STRUCTEx   *	m_pPower;
		void		     *	m_pData;		// 参见注二
	};
} RCV_DATA;


#pragma   pack(pop)					//去除内存对齐方式设置

/* 消息处理程序 DEMO
LONG OnStkDataOK(UINT wParam,LONG lParam)
{
	RCV_DATA * pHeader;
	pHeader = (RCV_DATA *)lParam;
	switch( wParam )
	{
	case RCV_REPORT:					//股票行情
		for(i=0; i<pHeader->m_nPacketNum; i++)
		{
			pHeader->m_pReportV3[i]...// 数据处理


			//CTime tm(pHeader->m_pReportV3[i].m_time);
			//CString jc=pHeader->m_pReportV3[i].m_szName;
			//CString jg;
			//jg.Format("%.2f",pHeader->m_pReportV3[i].m_fNewPrice);
			//AfxMessageBox(tm.Format ("%Y%m%d %H:%M:%S")+ " " + jc+ " " + jg);//显示时间 简称 最新价
		}
		break;

	case RCV_FILEDATA:					//文件
		switch(pHeader->m_wDataType)
		{
		case FILE_HISTORY_EX:				// 补日线数据
			break;
		case FILE_MINUTE_EX:				// 补分钟线数据
			break;
		case FILE_5MINUTE_EX:				// 补5分钟线数据
			break;
		case FILE_POWER_EX:				// 补充除权数据
			break;
		case FILE_BASE_EX:				// 基本资料文件,m_szFileName仅包含文件名
			break;
		case FILE_NEWS_EX:				// 新闻类,其类型由m_szFileName中子目录名来定
			break;
		}
		break;
	case RCV_FENBIDATA
		{
			RCV_FENBI*	pFb;
			pFb = (RCV_FENBI *)lParam;
			for ( i=0; i<pFb->m_nCount; i++ )
			{
				//数据处理
			}			
		}
		break;
	default:
		return 0;					// unknown data
	}
	return 1;
}
*/


//////////////////////////////////////////////////////////////////////////////////
//APIs
#ifdef __cplusplus
extern "C"{
#endif

//////////////////////////////////////////////////////////////////////////////////
// 注册函数

// 股票初始化
// 入口参数:
//		hWnd			处理消息的窗口句柄
//		Msg				用户自定义消息
//		nWorkMode		接口工作方式,参见工作方式类型定义
// 返回参数:
//		 1				成功	
//		-1				失败
// 注:
//		注册后,驱动程序会向处理窗口发送消息 
//		若股票接收没启动,则启动股票接收程序
int WINAPI	Stock_Init(HWND hWnd,UINT Msg,int nWorkMode);

// 退出,停止发送消息
// 入口参数:
//		hWnd			处理消息的窗口句柄,同 Stock_Init 的调用入口参数
//	返回参数:
//		 1				成功	
//		-1				失败
int WINAPI 	Stock_Quit(HWND hWnd);

// 激活接收程序,进行设置
// 入口参数:
//			bSetup		TRUE		显示窗口,进行设置
//						FALSE		隐含窗口
// 返回参数:
//			 1			成功
//			-1			失败
int	WINAPI  SetupReceiver(BOOL bSetup);

//	取得股票驱动信息
//	入口参数:
//			nInfo		索引
//			pBuf		缓冲区
//	出口参数:
//			nInfo == RI_IDSTRING,	返回特征字符串长度, pBuf 为特征字符串
//									如:	"银江网络"
//			nInfo == RI_IDCODE,		返回信息卡 ID 号, pBuf 为字符串形式的 ID 号
//									如:	0x78001234	  "78001234"
//			nInfo == RI_VERSION,	返回信息卡版本号, pBuf 为字符串版本
//									如:  1.00		  "1.00"		
//			nInfo == RI_V2SUPPORT,	返回是否支持深圳SJS库结构, pBuf无效

//			nInfo == RI_NETPCID	返回字符串长度， pBuf电脑硬件编号（用于验证合法用户）


//			另：当接口上的自动补充分时数据选项开启时，接口会根据分析软件的标题上的股票代码自动补充该股分时数据
DWORD WINAPI  GetStockDrvInfo(int nInfo,void * pBuf);


#ifdef __cplusplus
}
#endif


#endif // __STOCKDRV_H__

