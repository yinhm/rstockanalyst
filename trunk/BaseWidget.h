/************************************************************************/
/* 文件名称：BaseWidget.h
/* 创建时间：2012-11-16 10:59
/*
/* 描    述：所有绘制窗口的基类，支持拆分等操作
/************************************************************************/
#ifndef BASE_WIDGET_H
#define BASE_WIDGET_H
#include <QtGui>

class CBaseWidget : public QWidget
{
	Q_OBJECT
public:
	CBaseWidget(QWidget* parent = 0);
	~CBaseWidget(void);
};


#endif	//BASE_WIDGET_H