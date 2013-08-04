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
	QGridLayout* pMainLayout = new QGridLayout(this);

	{
		//左侧的模式选择列表
		m_pListColors = new QListWidget(this);
		pMainLayout->addWidget(m_pListColors,0,0,90,30);
		connect(m_pListColors,SIGNAL(itemSelectionChanged()),this,SLOT(onColorItemChanged()));

		QPushButton* pAddMode = new QPushButton(tr("添加"));
		pMainLayout->addWidget(pAddMode,90,0,5,15);
		connect(pAddMode,SIGNAL(clicked()),this,SLOT(onAddColorMode()));

		QPushButton* pRemoveMode = new QPushButton(tr("删除"));
		pMainLayout->addWidget(pRemoveMode,90,15,5,15);
		connect(pRemoveMode,SIGNAL(clicked()),this,SLOT(onRemoveColorMode()));
	}

	{
		//右侧的主显示窗口
		QGroupBox* pGroupBox = new QGroupBox(tr("选中模式颜色表"),this);
	//	QGridLayout* pRightLayout = new QGridLayout(this);
		for (int i=0;i<21;++i)
		{
			QLabel* pLabel = new QLabel(QString("%1").arg(i-10),this);
			pMainLayout->addWidget(pLabel,5+i*4,40,3,10);

			QPushButton* pBtn = new QPushButton(this);
			m_vColorButtons.push_back(pBtn);
			connect(pBtn,SIGNAL(clicked()),this,SLOT(onSetButtonColor()));
			pMainLayout->addWidget(pBtn,5+i*4,50,3,40);
		}

		pMainLayout->addWidget(pGroupBox,0,30,95,70);
	}

	{
		//最底下的 确定/取消 按钮
		QPushButton* pBtnOk = new QPushButton(tr("确定"));
		pMainLayout->addWidget(pBtnOk,95,0,5,50);
		connect(pBtnOk,SIGNAL(clicked()),this,SLOT(onBtnOk()));

		QPushButton* pBtnCancel = new QPushButton(tr("取消"));
		pMainLayout->addWidget(pBtnCancel,95,50,5,50);
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
	QList<QListWidgetItem*> listItems = m_pListColors->selectedItems();
	if(listItems.size()<1)
		return;
	QListWidgetItem* pItem = listItems[0];

	QString qsMode = pItem->text();
	if(!m_mapBlockColors.contains(qsMode))
		return;

	QVector<QColor> vColors = m_mapBlockColors[qsMode];
	if(vColors.size()<21)
		return;

	for (int i=0;i<21;++i)
	{
		QString qsStyle = QString("background-color: rgb(%1,%2,%3);")
			.arg(vColors[i].red())
			.arg(vColors[i].green())
			.arg(vColors[i].blue());

		//QString qsStyle = QString("background-color: #%1;")
		//	.arg(vColors[i].rgb(),8,16,QLatin1Char('0'));
		m_vColorButtons[i]->setStyleSheet(qsStyle);
		m_vColorButtons[i]->setProperty("c",vColors[i].rgb());
//		m_vColorButtons[i]->setData(21,vColors[i]);
	}
}

void CBlockColorSettingDlg::onSetButtonColor()
{
	QPushButton* pBtn = reinterpret_cast<QPushButton*>(sender());
	QColor initColor = QColor::fromRgb(pBtn->property("c").toUInt());

	QColor clr = QColorDialog::getColor(initColor,this);
	if(!clr.isValid())
		return;


	QString qsStyle = QString("background-color: rgb(%1,%2,%3);")
		.arg(clr.red())
		.arg(clr.green())
		.arg(clr.blue());

	pBtn->setStyleSheet(qsStyle);
	pBtn->setProperty("c",clr.rgb());

	int iIndex = m_vColorButtons.indexOf(pBtn);
	if(iIndex<0||iIndex>21)
		return;
	
	QList<QListWidgetItem*> listItems = m_pListColors->selectedItems();
	if(listItems.size()<1)
		return;

	QString qsMode = listItems[0]->text();
	m_mapBlockColors[qsMode][iIndex] = clr;
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
