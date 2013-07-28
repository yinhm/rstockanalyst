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

class CAbnomalSettingDlg : public QDialog
{
	Q_OBJECT
public:
	CAbnomalSettingDlg(const QMap<RAbnomalType,float>& map,QWidget* parent = 0);
	~CAbnomalSettingDlg(void);

public:
	QMap<RAbnomalType,float> getAbnomalMap();
protected:
	void initDlg();

protected slots:
	void onBtnOk();
	void onBtnCancel();

private:
	QLineEdit m_editHighIncrease;
	QLineEdit m_editHighTurnRatio;
	QLineEdit m_editHighVolumeRatio;

	QCheckBox m_ckHighIncrease;
	QCheckBox m_ckHighTurnRatio;
	QCheckBox m_ckHighVolumeRatio;

private:
	QMap<RAbnomalType,float> m_mapAbnomal;	//异常波动过滤器
};

#endif	//ABNORMAL_SETTING_DLG_H