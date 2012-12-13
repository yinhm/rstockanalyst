/************************************************************************/
/* 文件名称：BlockColorWidget.h
/* 创建时间：2012-12-13 10:03
/*
/* 描    述：色块颜色编辑窗口
/*           用于编辑色块窗口的显示颜色
/************************************************************************/
#ifndef BLOCK_COLOR_SETTING_DLG_H
#define BLOCK_COLOR_SETTING_DLG_H
#include <QtGui>

class CBlockColorSettingDlg : public QDialog
{
	Q_OBJECT
public:
	CBlockColorSettingDlg(QWidget* parent = 0);
	~CBlockColorSettingDlg(void);

	static CBlockColorSettingDlg* getDialog();

public:
	void reloadColorMode();
	bool saveColorMode();

protected slots:
	void onAddColorMode();
	void onRemoveColorMode();
	void onColorItemChanged();
	void onSetButtonColor();
	void onBtnOk();					//确定
	void onBtnCancel();				//取消

private:
	static CBlockColorSettingDlg* m_pDialog;
	QVector<QPushButton*> m_vColorButtons;
	QListWidget* m_pListColors;

	QMap<QString,QVector<QColor>> m_mapBlockColors;	//色块颜色种类。（通过百分比来所获取颜色的种类数）
};


#endif	//BLOCK_COLOR_WIDGET_H