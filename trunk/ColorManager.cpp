#include "StdAfx.h"
#include "ColorManager.h"

QVector<QColor> CColorManager::CommonColor;

void CColorManager::initAll()
{
	initCommonColor();
}

void CColorManager::initCommonColor()
{
	//http://blog.csdn.net/daichanglin/article/details/1563299
	CommonColor.push_back(QColor(220,20,60));		//猩红
	CommonColor.push_back(QColor(255,240,245));		//脸红的淡紫色
	CommonColor.push_back(QColor(128,0,128));		//紫色
	CommonColor.push_back(QColor(75,0,130));		//靛青
	CommonColor.push_back(QColor(0,0,205));			//适中的蓝色
	CommonColor.push_back(QColor(0,0,128));			//海军蓝
	CommonColor.push_back(QColor(176,196,222));		//淡钢蓝
	CommonColor.push_back(QColor(135,206,235));		//天蓝色
	CommonColor.push_back(QColor(0,191,255));		//深蓝色
	CommonColor.push_back(QColor(60,179,113));		//春天的绿色
	CommonColor.push_back(QColor(0,100,0));			//深绿色
	CommonColor.push_back(QColor(255,255,0));		//纯黄色
	CommonColor.push_back(QColor(128,128,0));		//橄榄色
	CommonColor.push_back(QColor(255,215,0));		//金色
	CommonColor.push_back(QColor(222,184,135));		//结实的树
	CommonColor.push_back(QColor(192,192,192));		//银白色
}
