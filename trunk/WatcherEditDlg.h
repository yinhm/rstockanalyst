/************************************************************************/
/* 文件名称：WatcherEditDlg.h
/* 创建时间：2013-06-08 14:03
/*
/* 描    述：用于编辑单个雷达监视对象
/************************************************************************/
#ifndef	WATCHER_EDIT_DLG_H
#define	WATCHER_EDIT_DLG_H
#include <QtGui>
#include "RadarWatcher.h"

class CWatcherEditDlg : public QDialog
{
	Q_OBJECT
public:
	CWatcherEditDlg(CRadarWatcher* pWatcher = NULL,QWidget* parent = NULL);
	~CWatcherEditDlg(void);


protected slots:
	void onBtnOk();
	void onBtnCancel();

private:
	//用于显示的控件
	QLineEdit m_editSec;
	QLineEdit m_editHold;
	QComboBox m_combType;
	QComboBox m_combBlock;
	QComboBox m_combDestBlock;
	//
	QPushButton m_btnOk;
	QPushButton m_btnCancel;

private:
	int m_iSec;									//监视间隔
	float m_fHold;								//阈值
	RadarType m_type;							//雷达类型
	CBlockInfoItem* m_pWatcherBlock;			//监视的板块
	CRadarWatcher* m_pWatcher;					//监视器
	CBlockInfoItem* m_pDestBlock;				//目标板块
};

#endif	//WATCHER_EDIT_DLG_H