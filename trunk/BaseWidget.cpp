/************************************************************************/
/* 文件名称：BaseWidget.cpp
/* 创建时间：2012-11-16 10:59
/*
/* 描    述：所有绘制窗口的基类，支持拆分等操作
/************************************************************************/
#include "StdAfx.h"
#include "BaseWidget.h"

CBaseWidget::CBaseWidget( QWidget* parent /*= 0*/ )
	: QWidget(parent)
{

}

CBaseWidget::~CBaseWidget(void)
{
}
