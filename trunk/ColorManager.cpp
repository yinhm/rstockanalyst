#include "StdAfx.h"
#include "ColorManager.h"

QVector<QColor> CColorManager::CommonColor;

QVector<QColor> CColorManager::DefaultColor;
QMap<QString,QVector<QColor>> CColorManager::BlockColors;


void CColorManager::initAll()
{
	initCommonColor();
	initBlockColors();
}

void CColorManager::reloadBlockColors()
{
	BlockColors.clear();

	QString qsDir = QString("%1/config/blockcolors").arg(qApp->applicationDirPath());
	QDir dir(qsDir);
	QFileInfoList list = dir.entryInfoList(QStringList()<<"*.clr",QDir::Files);

	foreach(const QFileInfo& info,list)
	{
		QVector<QColor> colors;
		QFile file(info.absoluteFilePath());
		if(!file.open(QFile::ReadOnly))
			continue;

		QString qsContent = file.readAll();
		QStringList listColors = qsContent.split("\n");
		if(listColors.size()<21)
			continue;

		foreach(const QString& clr,listColors)
		{
			QStringList RGBs = clr.trimmed().split(",");
			if(RGBs.size()<3)
				continue;

			int iR = RGBs[0].toInt();
			int iG = RGBs[1].toInt();
			int iB = RGBs[2].toInt();

			colors.push_back(QColor::fromRgb(iR,iG,iB));
		}

		if(colors.size()>20)
			BlockColors[info.completeBaseName()] = colors;

		file.close();
	}
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

void CColorManager::initBlockColors()
{
	//初始化基本的颜色表
	for (int i=0;i<COLOR_BLOCK_SIZE;++i)
	{
		int iColor = (255.0/(COLOR_BLOCK_SIZE-1))*i;
		DefaultColor.push_back(QColor::fromRgb(iColor,iColor,iColor));
	}

	reloadBlockColors();		//从文件加载颜色表
}

QStringList CColorManager::getBlockColorList()
{
	return BlockColors.keys();
}

QColor CColorManager::getBlockColor( const QString& mode,float fVal )
{
	int iColor = fVal*100+10;
	if(iColor>(COLOR_BLOCK_SIZE-1))
		iColor = (COLOR_BLOCK_SIZE-1);
	if(iColor<0)
		iColor = 0;

	if(BlockColors.contains(mode))
	{
		return BlockColors[mode][iColor];
	}
	else
	{
		return DefaultColor[iColor];
	}
}

QColor CColorManager::getBlockColor( const QString& mode,int index )
{
	if(index>(COLOR_BLOCK_SIZE-1))
		index = (COLOR_BLOCK_SIZE-1);
	if(index<0)
		index = 0;

	if(BlockColors.contains(mode))
	{
		return BlockColors[mode][index];
	}
	else
	{
		return DefaultColor[index];
	}
}
