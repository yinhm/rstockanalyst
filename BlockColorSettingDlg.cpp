/************************************************************************/
/* 文件名称：BlockColorWidget.cpp
/* 创建时间：2012-12-13 10:04
/*
/* 描    述：色块颜色编辑窗口
/*           用于编辑色块窗口的显示颜色
/************************************************************************/

#include "StdAfx.h"
#include "BlockColorSettingDlg.h"
#include "ColorManager.h"

CBlockColorSettingDlg* CBlockColorSettingDlg::m_pDialog = 0;

CBlockColorSettingDlg* CBlockColorSettingDlg::getDialog()
{
	if(m_pDialog==0)
		m_pDialog = new CBlockColorSettingDlg();

	return m_pDialog;
}

CBlockColorSettingDlg::CBlockColorSettingDlg( QWidget* parent /*= 0*/ )
	: QDialog(parent)
{
	QVBoxLayout* pMainLayout = new QVBoxLayout(this);

	QHBoxLayout* pCentLayout = new QHBoxLayout();
	QHBoxLayout* pLastLayout = new QHBoxLayout();
	pMainLayout->addLayout(pCentLayout,90);
	pMainLayout->addLayout(pLastLayout,10);

	{
		QVBoxLayout* pVLayout = new QVBoxLayout();
		pCentLayout->addLayout(pVLayout,50);
		//左侧的模式选择列表
		m_pListColors = new QListWidget(this);
		pVLayout->addWidget(m_pListColors,90);
		connect(m_pListColors,SIGNAL(itemSelectionChanged()),this,SLOT(onColorItemChanged()));

		QHBoxLayout* pHLayout = new QHBoxLayout();
		QPushButton* pAddMode = new QPushButton(tr("添加"));
		pHLayout->addWidget(pAddMode);
		connect(pAddMode,SIGNAL(clicked()),this,SLOT(onAddColorMode()));

		QPushButton* pRemoveMode = new QPushButton(tr("删除"));
		pHLayout->addWidget(pRemoveMode);
		connect(pRemoveMode,SIGNAL(clicked()),this,SLOT(onRemoveColorMode()));
		pVLayout->addLayout(pHLayout,10);
	}

	{
		QVBoxLayout* pVLayout = new QVBoxLayout();
		pCentLayout->addLayout(pVLayout,50);
		//右侧的主显示窗口
		QLabel* pLabel = new QLabel(tr("选中模式颜色表"),this);
		pVLayout->addWidget(pLabel);

		m_pListDetail = new QListWidget(this);
		m_pListDetail->setSelectionMode(QListWidget::MultiSelection);
		connect(m_pListDetail,SIGNAL(itemDoubleClicked(QListWidgetItem*)),this,SLOT(onClickedItemColor(QListWidgetItem*)));

		pVLayout->addWidget(m_pListDetail,80);

		QHBoxLayout* pHLayout = new QHBoxLayout();
		QPushButton* pAddMode = new QPushButton(tr("添加"));
		pHLayout->addWidget(pAddMode);
		connect(pAddMode,SIGNAL(clicked()),this,SLOT(onAddItemColor()));

		QPushButton* pRemoveMode = new QPushButton(tr("删除"));
		pHLayout->addWidget(pRemoveMode);
		connect(pRemoveMode,SIGNAL(clicked()),this,SLOT(onRemoveItemColor()));
		pVLayout->addLayout(pHLayout,10);
	}

	{
		//最底下的 确定/取消 按钮
		QPushButton* pBtnOk = new QPushButton(tr("确定"));
		pLastLayout->addWidget(pBtnOk);
		connect(pBtnOk,SIGNAL(clicked()),this,SLOT(onBtnOk()));

		QPushButton* pBtnCancel = new QPushButton(tr("取消"));
		pLastLayout->addWidget(pBtnCancel);
		connect(pBtnCancel,SIGNAL(clicked()),this,SLOT(onBtnCancel()));
	}
	this->setLayout(pMainLayout);

	m_pListColors->setSelectionMode(QListWidget::SingleSelection);

	reloadColorMode();

	this->setFixedHeight(650);
}

CBlockColorSettingDlg::~CBlockColorSettingDlg(void)
{
	delete m_pListColors;
}

void CBlockColorSettingDlg::reloadColorMode()
{
	m_mapBlockColors.clear();

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
		if(listColors.size()<1)
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

		if(colors.size()>0)
			m_mapBlockColors[info.completeBaseName()] = colors;

		file.close();
	}


	m_pListColors->clear();
	QStringList listKeys = m_mapBlockColors.keys();
	foreach(const QString& clr,listKeys)
	{
		m_pListColors->addItem(clr);
	}

	if(m_pListColors->count()>0)
	{
		m_pListColors->setItemSelected(m_pListColors->item(0),true);
	}
}

bool CBlockColorSettingDlg::saveColorMode()
{
	bool bRet = true;

	QString qsDir = QString("%1/config/blockcolors").arg(qApp->applicationDirPath());
	QDir dir(qsDir);
	QFileInfoList list = dir.entryInfoList(QStringList()<<"*.clr",QDir::Files);
	foreach(const QFileInfo& info,list)
	{
		if(!QFile::remove(info.absoluteFilePath()))
		{
			QMessageBox::warning(this,QString("删除文件失败"),QString("删除文件 \'%1\' 失败!").arg(info.absoluteFilePath()));
			bRet = false;
		}
	}

	foreach(const QString& str,m_mapBlockColors.keys())
	{
		QFile file(QString("%1/%2.clr").arg(qsDir).arg(str));
		if(!file.open(QFile::WriteOnly))
		{
			bRet = false;
			continue;
		}

		QVector<QColor> colors = m_mapBlockColors[str];
		for (int i=0;i<colors.size();++i)
		{
			file.write(QString("%1,%2,%3\r\n")
				.arg(colors[i].red())
				.arg(colors[i].green())
				.arg(colors[i].blue()).toLocal8Bit());
		}
		file.close();
	}

	return bRet;
}


void CBlockColorSettingDlg::onAddColorMode()
{
	QDialog dlg(this);
	QVBoxLayout layout(this);
	QLineEdit edit(this);
	QPushButton btnOk(this);
	dlg.setLayout(&layout);
	layout.addWidget(&edit);
	layout.addWidget(&btnOk);
	btnOk.setText(tr("确定"));
	connect(&btnOk,SIGNAL(clicked()),&dlg,SLOT(accept()));

	do 
	{
		int result = dlg.exec();
		if(QDialog::Accepted == result)
		{
			QString qsMode = edit.text().trimmed();
			if((!qsMode.isEmpty())&&(!m_mapBlockColors.contains(qsMode)))
			{
				QVector<QColor> vColors;
				for (int i=0;i<21;++i)
				{
					vColors.push_back(QColor(255,255,255));
				}
				m_mapBlockColors[qsMode] = vColors;
				m_pListColors->addItem(qsMode);
				m_pListColors->setItemSelected(m_pListColors->item(m_pListColors->count()-1),true);
				return;
			}
			else
			{
				QMessageBox::information(this,tr("同名"),tr("存在同名"));
			}
		}
		else
		{
			return;
		}
	} while (true);
}

void CBlockColorSettingDlg::onRemoveColorMode()
{
	QList<QListWidgetItem*> listItems = m_pListColors->selectedItems();
	if(listItems.size()<1)
		return;
	QListWidgetItem* pItem = listItems[0];
	QString qsMode = pItem->text();

	delete pItem;
	m_mapBlockColors.remove(qsMode);
}

void CBlockColorSettingDlg::onColorItemChanged()
{
	m_pListDetail->clear();

	QList<QListWidgetItem*> listItems = m_pListColors->selectedItems();
	if(listItems.size()<1)
		return;
	QListWidgetItem* pItem = listItems[0];

	QString qsMode = pItem->text();
	if(!m_mapBlockColors.contains(qsMode))
		return;

	QVector<QColor> vColors = m_mapBlockColors[qsMode];
	if(vColors.size()<1)
		return;

	foreach(const QColor& _c,vColors)
	{
		QListWidgetItem* pItem = new QListWidgetItem(m_pListDetail);
		pItem->setBackgroundColor(_c);

		QColor _c11 = pItem->backgroundColor();

		m_pListDetail->addItem(pItem);
	}
}

void CBlockColorSettingDlg::onBtnOk()
{
	if(saveColorMode())
	{
		CColorManager::reloadBlockColors();
	}
	hide();
}

void CBlockColorSettingDlg::onBtnCancel()
{
	reloadColorMode();
	hide();
}

void CBlockColorSettingDlg::onAddItemColor()
{
	QListWidgetItem* pItem = new QListWidgetItem();
	pItem->setBackgroundColor(QColor(255,0,0));
	m_pListDetail->addItem(pItem);
	updateCurMode();
}

void CBlockColorSettingDlg::onRemoveItemColor()
{
	QList<QListWidgetItem*> list = m_pListDetail->selectedItems();
	foreach(QListWidgetItem* pItem,list)
	{
		delete pItem;
	}
	updateCurMode();
}

void CBlockColorSettingDlg::onClickedItemColor( QListWidgetItem* pItem )
{
	if(pItem==NULL)
		return;

	QColor initColor = pItem->backgroundColor();

	QColor clr = QColorDialog::getColor(initColor,this);
	if(!clr.isValid())
		return;

	pItem->setBackgroundColor(clr);

	updateCurMode();
}

void CBlockColorSettingDlg::updateCurMode()
{
	QList<QListWidgetItem*> listItems = m_pListColors->selectedItems();
	if(listItems.size()<1)
		return;

	QString qsMode = listItems[0]->text();
	m_mapBlockColors[qsMode].clear();
	for(int i=0;i<m_pListDetail->count();++i)
	{
		QListWidgetItem* pItem = m_pListDetail->item(i);
		m_mapBlockColors[qsMode].append(pItem->backgroundColor());
	}
}
