/************************************************************************/
/* 文件名称：BlockColorWidget.h
/* 创建时间：2012-12-13 10:03
/*
/* 描    述：色块颜色编辑窗口
/*           用于编辑色块窗口的显示颜色
/************************************************************************/
#ifndef BLOCK_COLOR_WIDGET_H
#define BLOCK_COLOR_WIDGET_H
#include <QtGui>

class CBlockColorSettingDlg : public QDialog
{
	Q_OBJECT
public:
	CBlockColorSettingDlg(QWidget* parent = 0);
	~CBlockColorSettingDlg(void);
};


#endif	//BLOCK_COLOR_WIDGET_H