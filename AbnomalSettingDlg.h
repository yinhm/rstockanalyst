#ifndef ABNORMAL_SETTING_DLG_H
#define ABNORMAL_SETTING_DLG_H
#include <QtGui>

enum RAbnomalType
{
	HighIncrease = 1,		//高涨幅
	HighTurnRatio = 2,		//高换手率
	HighVolumeRatio = 4,	//高量比
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