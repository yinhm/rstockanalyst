#include "StdAfx.h"
#include "ColorManager.h"

QVector<uint> CColorManager::CommonColor;

CColorItem* CColorManager::m_pDefaultColor = NULL;
QMap<QString,CColorItem*> CColorManager::m_mapColors;


CColorItem::CColorItem( const QVector<uint>& vClrs )
	: m_vColors(vClrs)
{

}

CColorItem::~CColorItem()
{

}

uint CColorItem::getColor( const float& _f,const float& _abs/*=0.1*/ )
{
	if(m_vColors.size()<3)
		return rRGB(0,0,0);

	if(_f>=_abs)
	{
		return m_vColors.first();
	}
	else if(_f<=-_abs)
	{
		return m_vColors.last();
	}

	float fStep = (_abs*2/(m_vColors.size()-2));
	return m_vColors[(_abs-_f)/fStep];
}

uint CColorItem::getColor( const int& _v )
{
	if(m_vColors.size()<0)
		return rRGB(0,0,0);

	if(_v<0)
		return m_vColors.first();
	if(_v>=m_vColors.size())
		return m_vColors.last();

	return m_vColors[_v];
}


void CColorManager::initAll()
{
	initCommonColor();
	initBlockColors();
}

void CColorManager::reloadBlockColors()
{
	//清空以前的颜色表
	QMap<QString,CColorItem*>::iterator iter = m_mapColors.begin();
	while(iter!=m_mapColors.end())
	{
		CColorItem* pItem = iter.value();
		delete pItem;
		++iter;
	}
	m_mapColors.clear();


	QString qsDir = QString("%1/config/blockcolors").arg(qApp->applicationDirPath());
	QDir dir(qsDir);
	QFileInfoList list = dir.entryInfoList(QStringList()<<"*.clr",QDir::Files);

	foreach(const QFileInfo& info,list)
	{
		QVector<uint> colors;
		QFile file(info.absoluteFilePath());
		if(!file.open(QFile::ReadOnly))
			continue;

		QString qsContent = file.readAll();
		QStringList listColors = qsContent.split("\n");

		foreach(const QString& clr,listColors)
		{
			QStringList RGBs = clr.trimmed().split(",");
			if(RGBs.size()<3)
				continue;

			int iR = RGBs[0].toInt();
			int iG = RGBs[1].toInt();
			int iB = RGBs[2].toInt();

			colors.push_back(rRGB(iR,iG,iB));
		}

		m_mapColors[info.completeBaseName()] = new CColorItem(colors);

		file.close();
	}
}

void CColorManager::initCommonColor()
{
	//http://blog.csdn.net/daichanglin/article/details/1563299
	CommonColor.push_back(rRGB(220,20,60));		//猩红
	CommonColor.push_back(rRGB(255,240,245));		//脸红的淡紫色
	CommonColor.push_back(rRGB(128,0,128));		//紫色
	CommonColor.push_back(rRGB(75,0,130));		//靛青
	CommonColor.push_back(rRGB(0,0,205));			//适中的蓝色
	CommonColor.push_back(rRGB(0,0,128));			//海军蓝
	CommonColor.push_back(rRGB(176,196,222));		//淡钢蓝
	CommonColor.push_back(rRGB(135,206,235));		//天蓝色
	CommonColor.push_back(rRGB(0,191,255));		//深蓝色
	CommonColor.push_back(rRGB(60,179,113));		//春天的绿色
	CommonColor.push_back(rRGB(0,100,0));			//深绿色
	CommonColor.push_back(rRGB(255,255,0));		//纯黄色
	CommonColor.push_back(rRGB(128,128,0));		//橄榄色
	CommonColor.push_back(rRGB(255,215,0));		//金色
	CommonColor.push_back(rRGB(222,184,135));		//结实的树
	CommonColor.push_back(rRGB(192,192,192));		//银白色
}

void CColorManager::initBlockColors()
{
	//初始化基本的颜色表
	QVector<uint> vColors;
	for (int i=0;i<21;++i)
	{
		int iColor = (255.0/(20))*i;
		vColors.push_back(rRGB(iColor,iColor,iColor));
	}
	m_pDefaultColor = new CColorItem(vColors);

	reloadBlockColors();		//从文件加载颜色表
}

QStringList CColorManager::getBlockColorList()
{
	return m_mapColors.keys();
}

CColorItem* CColorManager::getColorItem( const QString& mode )
{
	if(m_mapColors.contains(mode))
	{
		return m_mapColors[mode];
	}

	return m_pDefaultColor;
}


uint CColorManager::getCommonColor( int index )
{
	return CommonColor[(index%(CommonColor.size()))];
}