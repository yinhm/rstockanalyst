/************************************************************************/
/* 文件名称：StockInfoWidget.cpp
/* 创建时间：2012-12-13 17:55
/*
/* 描    述：用于显示单只股票的行情信息，继承自CBaseWidget
/************************************************************************/

#include "StdAfx.h"
#include "StockInfoWidget.h"
#include "DataEngine.h"
#include "KeyWizard.h"

#define	R_DATA_COLOR(v)	( (v>=0) ? m_clrDataRed : m_clrDataBlue)

CStockInfoWidget::CStockInfoWidget( CBaseWidget* parent /*= 0*/ )
	: CBaseWidget(parent, WidgetStockInfo)
	, m_pStockItem(0)
	, m_iItemHeight(16)
	, m_clrGrid(176,0,0)
	, m_clrTitle(255,255,0)
	, m_clrDesc(192,192,192)
	, m_clrDataBlue(0,230,0)
	, m_clrDataRed(255,50,50)
	, m_clrDataNone(192,192,0)
{
	m_pMenuCustom = new QMenu(tr("行情信息菜单"));
	m_pMenuCustom->addAction(tr("设置股票代码"),this,SLOT(onSetStockCode()));
}

CStockInfoWidget::~CStockInfoWidget(void)
{
	delete m_pMenuCustom;
	disconnect(m_pStockItem,SIGNAL(stockItemReportChanged(const QString&)),this,SLOT(updateStockInfo(const QString&)));
}

bool CStockInfoWidget::loadPanelInfo( const QDomElement& eleWidget )
{
	if(!CBaseWidget::loadPanelInfo(eleWidget))
		return false;


	QDomElement eleCode = eleWidget.firstChildElement("code");
	if(eleCode.isElement())
		setStockCode(eleCode.text());

	return true;
}

bool CStockInfoWidget::savePanelInfo( QDomDocument& doc,QDomElement& eleWidget )
{
	if(!CBaseWidget::savePanelInfo(doc,eleWidget))
		return false;

	if(m_pStockItem)
	{
		//当前的股票值
		QDomElement eleCode = doc.createElement("code");
		eleCode.appendChild(doc.createTextNode(m_pStockItem->getCode()));
		eleWidget.appendChild(eleCode);
	}

	return true;
}

void CStockInfoWidget::setStockCode( const QString& code )
{
	setStockItem(CDataEngine::getDataEngine()->getStockInfoItem(code));
	return CBaseWidget::setStockCode(code);
}

void CStockInfoWidget::setStockItem( CStockInfoItem* pItem )
{
	if(pItem)
	{
		//移除所有和 updateStockInfo关联的 信号/槽
		if(m_pStockItem)
		{
			disconnect(m_pStockItem,SIGNAL(stockItemReportChanged(const QString&)),this,SLOT(updateStockInfo(const QString&)));
		}

		m_pStockItem = pItem;

		//建立更新机制
		//connect(pItem,SIGNAL(stockItemHistoryChanged(const QString&)),this,SLOT(updateStockInfo(const QString&)));
		//connect(pItem,SIGNAL(stockItemFenBiChanged(const QString&)),this,SLOT(updateStockInfo(const QString&)));
		connect(pItem,SIGNAL(stockItemReportChanged(const QString&)),this,SLOT(updateStockInfo(const QString&)));

		//更新行情信息
		update();
	}
}

void CStockInfoWidget::updateStockInfo( const QString& code )
{
	if(m_pStockItem&&m_pStockItem->getCode()!=code)
		return;

	update();
	//	resetTmpData();
}

void CStockInfoWidget::paintEvent( QPaintEvent* )
{
	QPainter p(this);
	QRect rtClient = this->rect();
	p.fillRect(rtClient,QColor(0,0,0));

	if(!m_pStockItem)
		return;


	int iCurY = rtClient.top();
	int iMidX = rtClient.width()/2;

	{
		//绘制标题
		QRect rtTitle(rtClient.left(),iCurY,rtClient.width(),m_iItemHeight);
		p.setPen(m_clrGrid);
		p.drawRect(rtTitle);
		p.setPen(m_clrTitle);
		p.drawText(rtTitle,Qt::AlignCenter,QString("%1 %2").arg(m_pStockItem->getCode()).arg(m_pStockItem->getName()));

		iCurY += m_iItemHeight;
	}

	p.setPen(m_clrGrid);
	p.drawLine(iMidX,iCurY,iMidX,rtClient.bottom());
	{
		//绘制 委比 委差
		QRect rtComm(rtClient.left(),iCurY,rtClient.width(),m_iItemHeight);
		QRect rtCommRatio(rtClient.left()+2,iCurY,iMidX-3,m_iItemHeight);	//委比
		QRect rtCommSent(iMidX+2,iCurY,rtCommRatio.width(),m_iItemHeight); //委差
		p.setPen(m_clrGrid);
		p.drawRect(rtComm);

		p.setPen(m_clrDesc);
		p.drawText(rtCommRatio,Qt::AlignLeft|Qt::AlignVCenter,tr("委比"));
		p.drawText(rtCommSent,Qt::AlignLeft|Qt::AlignVCenter,tr("委差"));

		p.setPen(m_clrDataRed);
		p.drawText(rtCommRatio,Qt::AlignRight|Qt::AlignVCenter,QString("%1%").arg(m_pStockItem->getCommRatio(),0,'f',2));
		p.drawText(rtCommSent,Qt::AlignRight|Qt::AlignVCenter,QString("%1").arg(m_pStockItem->getCommSent(),0,'f',2));

		iCurY += m_iItemHeight;
	}
	{
		//委卖 委买
		qRcvReportData* pReport = m_pStockItem->getCurrentReport();
		if(pReport)
		{
			//委卖
			{
				QRect rtSell(rtClient.left(),iCurY,rtClient.right(),m_iItemHeight*5);
				p.setPen(m_clrGrid);
				p.drawRect(rtSell);

				{
					QRect rtLeft(rtSell.left()+2,iCurY,iMidX-3,m_iItemHeight);
					QRect rtRight(iMidX+2,iCurY,rtLeft.width(),m_iItemHeight);

					p.setPen(m_clrDesc);
					p.drawText(rtLeft,Qt::AlignLeft|Qt::AlignVCenter,
						QString("委卖5"));

					p.setPen(m_clrDataRed);
					p.drawText(rtLeft,Qt::AlignRight|Qt::AlignVCenter,
						QString("%1").arg(pReport->fSellPrice5,0,'f',2));

					p.setPen(m_clrDataNone);
					p.drawText(rtRight,Qt::AlignRight|Qt::AlignVCenter,
						QString("%1").arg(pReport->fSellVolume5));

					iCurY += m_iItemHeight;
				}
				{
					QRect rtLeft(rtSell.left()+2,iCurY,iMidX-3,m_iItemHeight);
					QRect rtRight(iMidX+2,iCurY,rtLeft.width(),m_iItemHeight);

					p.setPen(m_clrDesc);
					p.drawText(rtLeft,Qt::AlignLeft|Qt::AlignVCenter,
						QString("委卖4"));

					p.setPen(m_clrDataRed);
					p.drawText(rtLeft,Qt::AlignRight|Qt::AlignVCenter,
						QString("%1").arg(pReport->fSellPrice4,0,'f',2));

					p.setPen(m_clrDataNone);
					p.drawText(rtRight,Qt::AlignRight|Qt::AlignVCenter,
						QString("%1").arg(pReport->fSellVolume4));

					iCurY += m_iItemHeight;
				}
				for (int i=2;i>=0;--i)
				{
					QRect rtLeft(rtSell.left()+2,iCurY,iMidX-3,m_iItemHeight);
					QRect rtRight(iMidX+2,iCurY,rtLeft.width(),m_iItemHeight);

					p.setPen(m_clrDesc);
					p.drawText(rtLeft,Qt::AlignLeft|Qt::AlignVCenter,
						QString("委卖%1").arg(i+1));

					p.setPen(m_clrDataRed);
					p.drawText(rtLeft,Qt::AlignRight|Qt::AlignVCenter,
						QString("%1").arg(pReport->fSellPrice[i],0,'f',2));

					p.setPen(m_clrDataNone);
					p.drawText(rtRight,Qt::AlignRight|Qt::AlignVCenter,
						QString("%1").arg(pReport->fSellVolume[i]));

					iCurY += m_iItemHeight;
				}
			}

			//委买
			{
				QRect rtBuy(rtClient.left(),iCurY,rtClient.right(),m_iItemHeight*5);
				p.setPen(m_clrGrid);
				p.drawRect(rtBuy);

				for (int i=0;i<3;++i)
				{
					QRect rtLeft(rtBuy.left()+2,iCurY,iMidX-3,m_iItemHeight);
					QRect rtRight(iMidX+2,iCurY,rtLeft.width(),m_iItemHeight);

					p.setPen(m_clrDesc);
					p.drawText(rtLeft,Qt::AlignLeft|Qt::AlignVCenter,
						QString("委买%1").arg(i+1));

					p.setPen(m_clrDataRed);
					p.drawText(rtLeft,Qt::AlignRight|Qt::AlignVCenter,
						QString("%1").arg(pReport->fBuyPrice[i],0,'f',2));

					p.setPen(m_clrDataNone);
					p.drawText(rtRight,Qt::AlignRight|Qt::AlignVCenter,
						QString("%1").arg(pReport->fBuyVolume[i]));

					iCurY += m_iItemHeight;
				}
				{
					QRect rtLeft(rtBuy.left()+2,iCurY,iMidX-3,m_iItemHeight);
					QRect rtRight(iMidX+2,iCurY,rtLeft.width(),m_iItemHeight);

					p.setPen(m_clrDesc);
					p.drawText(rtLeft,Qt::AlignLeft|Qt::AlignVCenter,
						QString("委买4"));

					p.setPen(m_clrDataRed);
					p.drawText(rtLeft,Qt::AlignRight|Qt::AlignVCenter,
						QString("%1").arg(pReport->fBuyPrice4,0,'f',2));

					p.setPen(m_clrDataNone);
					p.drawText(rtRight,Qt::AlignRight|Qt::AlignVCenter,
						QString("%1").arg(pReport->fBuyVolume4));

					iCurY += m_iItemHeight;
				}
				{
					QRect rtLeft(rtBuy.left()+2,iCurY,iMidX-3,m_iItemHeight);
					QRect rtRight(iMidX+2,iCurY,rtLeft.width(),m_iItemHeight);

					p.setPen(m_clrDesc);
					p.drawText(rtLeft,Qt::AlignLeft|Qt::AlignVCenter,
						QString("委买5"));

					p.setPen(m_clrDataRed);
					p.drawText(rtLeft,Qt::AlignRight|Qt::AlignVCenter,
						QString("%1").arg(pReport->fBuyPrice5,0,'f',2));

					p.setPen(m_clrDataNone);
					p.drawText(rtRight,Qt::AlignRight|Qt::AlignVCenter,
						QString("%1").arg(pReport->fBuyVolume5));

					iCurY += m_iItemHeight;
				}
			}
		}

	}

	{
		//当前行情
		QRect rtInfo(rtClient.left(),iCurY,rtClient.width(),m_iItemHeight*5);
		p.setPen(m_clrGrid);
		p.drawRect(rtInfo);
		{
			//现价，今开
			QRect rtLeft(rtInfo.left()+2,iCurY,iMidX-3,m_iItemHeight);
			QRect rtRight(iMidX+2,iCurY,rtLeft.width(),m_iItemHeight);
			p.setPen(m_clrDesc);
			p.drawText(rtLeft,Qt::AlignLeft|Qt::AlignVCenter,tr("现价"));
			p.drawText(rtRight,Qt::AlignLeft|Qt::AlignVCenter,tr("今开"));

			p.setPen(R_DATA_COLOR(m_pStockItem->getNewPrice()-m_pStockItem->getLastClose()));
			p.drawText(rtLeft,Qt::AlignRight|Qt::AlignVCenter,QString("%1").arg(m_pStockItem->getNewPrice(),0,'f',2));
			p.setPen(R_DATA_COLOR(m_pStockItem->getOpenPrice()-m_pStockItem->getLastClose()));
			p.drawText(rtRight,Qt::AlignRight|Qt::AlignVCenter,QString("%1").arg(m_pStockItem->getOpenPrice(),0,'f',2));

			iCurY += m_iItemHeight;
		}
		{
			//涨跌，最高
			QRect rtLeft(rtInfo.left()+2,iCurY,iMidX-3,m_iItemHeight);
			QRect rtRight(iMidX+2,iCurY,rtLeft.width(),m_iItemHeight);
			p.setPen(m_clrDesc);
			p.drawText(rtLeft,Qt::AlignLeft|Qt::AlignVCenter,tr("涨跌"));
			p.drawText(rtRight,Qt::AlignLeft|Qt::AlignVCenter,tr("最高"));

			p.setPen(R_DATA_COLOR(m_pStockItem->getPriceFluctuate()));
			p.drawText(rtLeft,Qt::AlignRight|Qt::AlignVCenter,QString("%1").arg(m_pStockItem->getPriceFluctuate(),0,'f',2));
			p.setPen(R_DATA_COLOR(m_pStockItem->getHighPrice()-m_pStockItem->getLastClose()));
			p.drawText(rtRight,Qt::AlignRight|Qt::AlignVCenter,QString("%1").arg(m_pStockItem->getHighPrice(),0,'f',2));

			iCurY += m_iItemHeight;
		}
		{
			//涨幅，最低
			QRect rtLeft(rtInfo.left()+2,iCurY,iMidX-3,m_iItemHeight);
			QRect rtRight(iMidX+2,iCurY,rtLeft.width(),m_iItemHeight);
			p.setPen(m_clrDesc);
			p.drawText(rtLeft,Qt::AlignLeft|Qt::AlignVCenter,tr("涨幅"));
			p.drawText(rtRight,Qt::AlignLeft|Qt::AlignVCenter,tr("最低"));

			p.setPen(R_DATA_COLOR(m_pStockItem->getIncrease()));
			p.drawText(rtLeft,Qt::AlignRight|Qt::AlignVCenter,QString("%1%").arg(m_pStockItem->getIncrease(),0,'f',2));
			p.setPen(R_DATA_COLOR(m_pStockItem->getLowPrice()-m_pStockItem->getLastClose()));
			p.drawText(rtRight,Qt::AlignRight|Qt::AlignVCenter,QString("%1").arg(m_pStockItem->getLowPrice(),0,'f',2));

			iCurY += m_iItemHeight;
		}
		{
			//总量，量比
			QRect rtLeft(rtInfo.left()+2,iCurY,iMidX-3,m_iItemHeight);
			QRect rtRight(iMidX+2,iCurY,rtLeft.width(),m_iItemHeight);
			p.setPen(m_clrDesc);
			p.drawText(rtLeft,Qt::AlignLeft|Qt::AlignVCenter,tr("总量"));
			p.drawText(rtRight,Qt::AlignLeft|Qt::AlignVCenter,tr("量比"));

			p.setPen(m_clrDataNone);
			p.drawText(rtLeft,Qt::AlignRight|Qt::AlignVCenter,QString("%1").arg(m_pStockItem->getTotalAmount()));
			p.setPen(R_DATA_COLOR(m_pStockItem->getVolumeRatio()));
			p.drawText(rtRight,Qt::AlignRight|Qt::AlignVCenter,QString("%1").arg(m_pStockItem->getVolumeRatio(),0,'f',2));

			iCurY += m_iItemHeight;
		}
		{
			//外盘，内盘
			QRect rtLeft(rtInfo.left()+2,iCurY,iMidX-3,m_iItemHeight);
			QRect rtRight(iMidX+2,iCurY,rtLeft.width(),m_iItemHeight);
			p.setPen(m_clrDesc);
			p.drawText(rtLeft,Qt::AlignLeft|Qt::AlignVCenter,tr("外盘"));
			p.drawText(rtRight,Qt::AlignLeft|Qt::AlignVCenter,tr("内盘"));

			p.setPen(m_clrDataRed);
			p.drawText(rtLeft,Qt::AlignRight|Qt::AlignVCenter,QString("%1").arg(m_pStockItem->getSellVOL()));
			p.drawText(rtRight,Qt::AlignRight|Qt::AlignVCenter,QString("%1").arg(m_pStockItem->getBuyVOL()));

			iCurY += m_iItemHeight;
		}
	}
	{
		//特殊信息
		QRect rtInfo(rtClient.left(),iCurY,rtClient.width(),rtClient.bottom()-iCurY);
		p.setPen(m_clrGrid);
		p.drawRect(rtInfo);
		{
			//换手，股本
			QRect rtLeft(rtInfo.left()+2,iCurY,iMidX-3,m_iItemHeight);
			QRect rtRight(iMidX+2,iCurY,rtLeft.width(),m_iItemHeight);
			p.setPen(m_clrDesc);
			p.drawText(rtLeft,Qt::AlignLeft|Qt::AlignVCenter,tr("换手"));
			p.drawText(rtRight,Qt::AlignLeft|Qt::AlignVCenter,tr("股本"));

			p.drawText(rtLeft,Qt::AlignRight|Qt::AlignVCenter,QString("%1%").arg(m_pStockItem->getTurnRatio(),0,'f',2));
			p.drawText(rtRight,Qt::AlignRight|Qt::AlignVCenter,QString("%1").arg(m_pStockItem->getZGB()));

			iCurY += m_iItemHeight;
		}
		{
			//净资，流通
			QRect rtLeft(rtInfo.left()+2,iCurY,iMidX-3,m_iItemHeight);
			QRect rtRight(iMidX+2,iCurY,rtLeft.width(),m_iItemHeight);
			p.setPen(m_clrDesc);
			p.drawText(rtLeft,Qt::AlignLeft|Qt::AlignVCenter,tr("净资"));
			p.drawText(rtRight,Qt::AlignLeft|Qt::AlignVCenter,tr("流通"));

			p.drawText(rtLeft,Qt::AlignRight|Qt::AlignVCenter,QString("%1").arg(m_pStockItem->getMgjzc(),0,'f',2));
			p.drawText(rtRight,Qt::AlignRight|Qt::AlignVCenter,QString("%1").arg(m_pStockItem->getLTSZ()));

			iCurY += m_iItemHeight;
		}
		{
			//收益（三），PE（动）
			QRect rtLeft(rtInfo.left()+2,iCurY,iMidX-3,m_iItemHeight);
			QRect rtRight(iMidX+2,iCurY,rtLeft.width(),m_iItemHeight);
			p.setPen(m_clrDesc);
			p.drawText(rtLeft,Qt::AlignLeft|Qt::AlignVCenter,tr("收益(三)"));
			p.drawText(rtRight,Qt::AlignLeft|Qt::AlignVCenter,tr("PE(动)"));

			p.drawText(rtLeft,Qt::AlignRight|Qt::AlignVCenter,QString("%1").arg(m_pStockItem->getMgsy(),0,'f',3));
			p.drawText(rtRight,Qt::AlignRight|Qt::AlignVCenter,QString("%1").arg(m_pStockItem->getPERatio(),0,'f',2));

			iCurY += m_iItemHeight;
		}

	}
}

QMenu* CStockInfoWidget::getCustomMenu()
{
	QAction* pAction = m_pMenu->menuAction();
	if(!m_pMenuCustom->actionGeometry(pAction).isValid())
		m_pMenuCustom->addMenu(m_pMenu);

	return m_pMenuCustom;
}

void CStockInfoWidget::onSetStockCode()
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

	if(QDialog::Accepted != dlg.exec())
		return;

	QString code = edit.text();
	setStockCode(code);
}

void CStockInfoWidget::getKeyWizData( const QString& keyword,QList<KeyWizData*>& listRet )
{
	foreach(CStockInfoItem* pItem,CDataEngine::getDataEngine()->getStockInfoList())
	{
		if(pItem->isMatch(keyword))
		{
			KeyWizData* pData = new KeyWizData;
			pData->cmd = CKeyWizard::CmdStock;
			pData->arg = pItem;
			pData->desc = QString("%1 %2").arg(pItem->getCode()).arg(pItem->getName());
			listRet.push_back(pData);
			if(listRet.size()>20)
				return;
		}
	}

	return CBaseWidget::getKeyWizData(keyword,listRet);
}

void CStockInfoWidget::keyWizEntered( KeyWizData* pData )
{
	if(pData->cmd == CKeyWizard::CmdStock)
	{
		setStockItem(reinterpret_cast<CStockInfoItem*>(pData->arg));
		return;
	}

	return CBaseWidget::keyWizEntered(pData);
}
