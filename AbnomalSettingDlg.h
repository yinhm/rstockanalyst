#ifndef ABNORMAL_SETTING_DLG_H
#define ABNORMAL_SETTING_DLG_H
#include <QtGui>

enum RAbnomalType
{
	HighIncrease = 1,		//高涨幅
	LowIncrease,			//低涨幅（跌）
	HighTurnRatio,			//高换手率
	HighVolumeRatio,		//高量比
	HighAvgVolume,			//每笔均量 ？
	HighPrice,				//价格大于某值
	LowPrice,				//价格小于某值
	HighLTSZ,				//高流通盘
	LowLTSZ,				//低流通盘
	HighMGSY,				//高每股收益
	HighPERatio,			//高市盈率
	HighZYYWRatio,			//高主营业务增长率 ？
	HighYJYZ,				//高业绩预增
	HighYJZZ,				//高业绩增长
	HighYJHBZZ,				//高业绩环比增长连续4个季度
	HighJZCSYL,				//净资产收益率   >=
	HighBuyVolume,			//委买1+2+3>=
	HighSellVolume,			//委卖1+2+3>=
	HighCommRatio,			//委比 >=
};

enum RAbnomalEqual
{
	AbnomalLesser = -1,		//小于等于
	AbnomalEqual = 0,		//等于
	AbnomalGreater = 1,		//大于等于
};

struct RAbnomalValue
{
	RAbnomalType _T;		//种类
	RAbnomalEqual _E;		//对比
	float _Default;			//默认值
	QString _Desc;			//描述
	RAbnomalValue(RAbnomalType _t,RAbnomalEqual _e,QString _d,float _default)
	{
		_T = _t;
		_E = _e;
		_Desc = _d;
		_Default = _default;
	}
};

class CAbnomalSettingDlg : public QDialog
{
	Q_OBJECT
public:
	static void initAbnomals();
	static void ShowAbnomalDlg(QMap<RAbnomalType,float>& map,QWidget* parent = 0);

protected:
	CAbnomalSettingDlg(const QMap<RAbnomalType,float>& map,QWidget* parent = 0);
	~CAbnomalSettingDlg(void);

protected:
	QMap<RAbnomalType,float> getAbnomalMap();
	void initDlg();

protected slots:
	void onBtnOk();
	void onBtnCancel();

private:
	QMap<RAbnomalType,QPair<QLineEdit*,QCheckBox*>> m_mapCtrls;		//所有的LineEdit
	QMap<RAbnomalType,float> m_mapAbnomal;	//异常波动过滤器

private:
	static QMap<RAbnomalType,RAbnomalValue*> m_mapAbnomals;		//
	static bool m_bInit;
};

#endif	//ABNORMAL_SETTING_DLG_H