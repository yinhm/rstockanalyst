#include "StdAfx.h"
#include "WatcherEditDlg.h"
#include "DataEngine.h"


CWatcherEditDlg::CWatcherEditDlg( CRadarWatcher* pWatcher /*= NULL*/,QWidget* parent /*= NULL*/ )
	: QDialog(parent)
	, m_iSec(30)
	, m_fHold(0)
	, m_type(BigIncrease)
	, m_pWatcherBlock(0)
	, m_pWatcher(pWatcher)
	, m_pDestBlock(0)
{
	if(m_pWatcher)
	{
		m_iSec = pWatcher->getSec();
		m_fHold = pWatcher->getHold();
		m_type = pWatcher->getType();
		m_pWatcherBlock = pWatcher->getBlock();
		m_pDestBlock = pWatcher->getDestBlock();
	}

	{
		//初始化雷达类型
		m_combType.addItem(CRadarManager::getTypeName(BigVolumn),BigVolumn);
		m_combType.addItem(CRadarManager::getTypeName(BigIncrease),BigIncrease);
		m_combType.addItem(CRadarManager::getTypeName(MaxPrice),MaxPrice);
		m_combType.addItem(CRadarManager::getTypeName(MinPrice),MinPrice);

		//设置当前选中项
		if(m_pWatcher)
		{
			m_combType.setCurrentIndex(m_combType.findData(m_pWatcher->getType()));
			m_combType.setEnabled(false);
		}
		else
		{
			m_combType.setCurrentIndex(0);
		}
	}

	{
		//初始化所有的板块
		QList<CBlockInfoItem*> list = CDataEngine::getDataEngine()->getStockBlocks();
		foreach(CBlockInfoItem* pItem,list)
		{
			if(pItem->getStockCount()>0)
			{
				m_combBlock.addItem(pItem->getName(),reinterpret_cast<uint>(pItem));
			}
		}

		if(m_pWatcher)
		{
			m_combBlock.setCurrentIndex(m_combBlock.findData(reinterpret_cast<uint>(m_pWatcher->getBlock())));
		}
		else
		{
			m_combBlock.setCurrentIndex(0);
		}
	}
	{
		CBlockInfoItem* pCustomBlock = CDataEngine::getDataEngine()->getCustomBlock();
		if(pCustomBlock)
		{
			QList<CBlockInfoItem*> list = pCustomBlock->getBlockList();
			foreach(CBlockInfoItem* pItem,list)
			{
				m_combDestBlock.addItem(pItem->getName(),reinterpret_cast<uint>(pItem));
			}
		}
		if(m_pDestBlock)
		{
			m_combDestBlock.setCurrentIndex(m_combDestBlock.findData(reinterpret_cast<uint>(m_pDestBlock)));
		}
	}

	m_editSec.setText(QString("%1").arg(m_iSec));
	m_editSec.setValidator(new QIntValidator());
	m_editHold.setText(QString("%1").arg(m_fHold));
	m_editHold.setValidator(new QDoubleValidator());
	m_btnOk.setText(tr("确定"));
	m_btnCancel.setText(tr("取消"));

	connect(&m_btnOk,SIGNAL(clicked()),this,SLOT(onBtnOk()));
	connect(&m_btnCancel,SIGNAL(clicked()),this,SLOT(onBtnCancel()));
	

	QGridLayout* pLayout = new QGridLayout(this);
	pLayout->addWidget(new QLabel("类型:",this),0,0,1,1);
	pLayout->addWidget(&m_combType,0,1,1,2);
	pLayout->addWidget(new QLabel("周期:",this),1,0,1,1);
	pLayout->addWidget(&m_editSec,1,1,1,2);
	pLayout->addWidget(new QLabel("阈值:",this),2,0,1,1);
	pLayout->addWidget(&m_editHold,2,1,1,2);
	pLayout->addWidget(new QLabel("板块:",this),3,0,1,1);
	pLayout->addWidget(&m_combBlock,3,1,1,2);
	pLayout->addWidget(new QLabel("目标板块:",this),4,0,1,1);
	pLayout->addWidget(&m_combDestBlock,4,1,1,2);

	pLayout->addWidget(&m_btnOk,5,1,1,1);
	pLayout->addWidget(&m_btnCancel,5,2,1,1);
	setLayout(pLayout);
}


CWatcherEditDlg::~CWatcherEditDlg(void)
{
}

void CWatcherEditDlg::onBtnOk()
{
	m_iSec = m_editSec.text().toInt();
	m_fHold = m_editHold.text().toFloat();
	{
		int iIndex = m_combType.currentIndex();
		if(iIndex>=0)
		{
			m_type = static_cast<RadarType>(m_combType.itemData(iIndex).toInt());
		}
	}
	{
		int iIndex = m_combBlock.currentIndex();
		if(iIndex>=0)
		{
			m_pWatcherBlock = reinterpret_cast<CBlockInfoItem*>(m_combBlock.itemData(iIndex).toUInt());
		}
	}
	{
		int iIndex = m_combDestBlock.currentIndex();
		if(iIndex>=0)
		{
			m_pDestBlock = reinterpret_cast<CBlockInfoItem*>(m_combDestBlock.itemData(iIndex).toUInt());
		}
	}


	if(m_pWatcher)
	{
		m_pWatcher->setSec(m_iSec);
		m_pWatcher->setHold(m_fHold);
		m_pWatcher->setBlock(m_pWatcherBlock);
		m_pWatcher->setDestBlock(m_pDestBlock);
	}
	else
	{
		CRadarManager::getRadarManager()->createRadarWatcher(m_pWatcherBlock,m_type,m_iSec,m_fHold,m_pDestBlock);
	}
	accept();
}

void CWatcherEditDlg::onBtnCancel()
{
	reject();
}
