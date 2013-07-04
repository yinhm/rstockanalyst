#include "StdAfx.h"
#include "ColorBlockWidget.h"
#include "DataEngine.h"
#include "ColorManager.h"
#include "BlockInfoItem.h"
#include "RLuaEx.h"
#include "KeyWizard.h"
#include "MainWindow.h"

#define BLOCK_CMD_SET	1

#define	RCB_OFFSET_Y	2
#define RCB_OFFSET_LEFT	0

QVector<float> getVectorForTime(const QMap<time_t,int>& mapTimes,const QList<RStockData*>& listDatas, CColorBlockWidget::RShowType _t, CAbstractStockItem* pStock)
{
	QVector<float> vRet;

//	fVolumeRatio = (pCurrentReport->fVolume)/((fLast5Volume/((CDataEngine::getOpenSeconds()/60)*5))*(tmSeconds/60));
//	((pCurrentReport->fVolume/100)/baseInfo.fLtAg)*100;

	QMap<time_t,int>::const_iterator iter = mapTimes.begin();
	QList<RStockData*>::const_iterator iterD = listDatas.begin();
	switch(_t)
	{
	case CColorBlockWidget::ShowIncrease:
		{
			RStockData* pLastData = 0;

			while(iter!=mapTimes.end())
			{
				if(iterD!=listDatas.end())
				{
					uint iT = iter.key()/300;
					uint iTD = (*iterD)->tmTime/300;
					if( iT == iTD)
					{
						//计算
						if(pLastData)
						{
							vRet.push_back((((*iterD)->fClose - pLastData->fClose) / pLastData->fClose));
						}
						else
						{
							vRet.push_back(0);
						}
						pLastData = *iterD;
						++iter;
						++iterD;
					}
					else if(iT<iTD)
					{
						vRet.push_back(0);
						++iter;
					}
					else
					{
						++iterD;
					}
				}
				else
				{
					vRet.push_back(0);
					++iter;
				}
			}
		}
		break;
	case CColorBlockWidget::ShowTurnRatio:
		{
			float fLTAG = pStock->getLtag();
			if(fLTAG<0.1)
				break;

			while(iter!=mapTimes.end())
			{
				if(iterD!=listDatas.end())
				{
					uint iT = iter.key()/300;
					uint iTD = (*iterD)->tmTime/300;
					if( iT == iTD)
					{
						//计算
						vRet.push_back((((*iterD)->fVolume) / fLTAG)*10);
						++iter;
						++iterD;
					}
					else if(iT<iTD)
					{
						vRet.push_back(0);
						++iter;
					}
					else
					{
						++iterD;
					}
				}
				else
				{
					vRet.push_back(0);
					++iter;
				}
			}
		}
		break;
	case CColorBlockWidget::ShowVolumeRatio:
		{
			float fLast5Volume = pStock->getLast5Volume();
			if(fLast5Volume<0.1)
				break;

			while(iter!=mapTimes.end())
			{
				if(iterD!=listDatas.end())
				{
					uint iT = iter.key()/300;
					uint iTD = (*iterD)->tmTime/300;
					if( iT == iTD)
					{
						//计算
						vRet.push_back((((*iterD)->fVolume) / ((fLast5Volume/((CDataEngine::getOpenSeconds()/60)*5))*(5))));
						++iter;
						++iterD;
					}
					else if(iT<iTD)
					{
						vRet.push_back(0);
						++iter;
					}
					else
					{
						++iterD;
					}
				}
				else
				{
					vRet.push_back(0);
					++iter;
				}
			}
		}
		break;
	}

	return vRet;
}


CColorBlockWidget::CColorBlockWidget( CBaseWidget* parent /*= 0*/ )
	: CBaseBlockWidget(parent,WidgetSColorBlock)
	, m_iTitleHeight(16)
	, m_iBottomHeight(16)
	, showStockIndex(0)
	, m_pSelectedStock(0)
	, m_pBlock(0)
	, m_bShowIncrease(true)
	, m_bShowTurnRatio(true)
	, m_bShowVolumeRatio(true)
{
	{
		//初始化显示类型
		m_listShowOp.push_back(RWidgetOpData(ShowIncrease,"vsc","显示涨幅（颜色）"));
		m_listShowOp.push_back(RWidgetOpData(ShowTurnRatio,"vsh","显示换手率（高度）"));
		m_listShowOp.push_back(RWidgetOpData(ShowVolumeRatio,"vsw","显示量比（宽度）"));
	}
	//设置当前的表达式
//	m_pMenuCustom->addAction(tr("设置当前的表达式"),
//		this,SLOT(onSetExpression()));

	//设置置顶股票
	m_pMenuCustom->addAction(tr("设置置顶股票"),this,SLOT(onSetTopStock()));
	m_pMenuCustom->addAction(tr("移除置顶股票"),this,SLOT(onRemoveTopStock()));

	//设置显示类型
	m_pMenuShowType = m_pMenuCustom->addMenu("设置显示类型");
	{
		foreach(const RWidgetOpData& _d,m_listShowOp)
		{
			m_pMenuShowType->addAction(_d.desc,this,SLOT(onSetShowType()))->setData(_d.value);
		}
	}


	connect(&m_timerUpdateUI,SIGNAL(timeout()),this,SLOT(updateColorBlockData()));
	m_timerUpdateUI.start(1000);
}

CColorBlockWidget::~CColorBlockWidget(void)
{
	clearTmpData();
}

bool CColorBlockWidget::loadPanelInfo( const QDomElement& eleWidget )
{
	if(!CBaseBlockWidget::loadPanelInfo(eleWidget))
		return false;

	updateColorBlockData();
	//当前的板块名称
	QDomElement eleBlock = eleWidget.firstChildElement("block");
	if(eleBlock.isElement())
	{
		setBlock(eleBlock.text());
	}

	//当前的表达式
	QDomElement eleShowType = eleWidget.firstChildElement("ShowType");
	if(eleShowType.isElement())
	{
		//子节点
		m_bShowIncrease = eleShowType.attribute("increase","1").toInt();
		m_bShowTurnRatio = eleShowType.attribute("turnratio","1").toInt();
		m_bShowVolumeRatio = eleShowType.attribute("volumeratio","1").toInt();
	}


	{
		//置顶股票
		QDomElement eleTopStocks = eleWidget.firstChildElement("TopStocks");
		if(eleTopStocks.isElement())
		{
			QDomElement eleStock = eleTopStocks.firstChildElement("Stock");
			while(eleStock.isElement())
			{
				CStockInfoItem* pItem = CDataEngine::getDataEngine()->getStockInfoItem(eleStock.text());
				if(pItem)
				{
					m_listTopStocks.append(pItem);
				}

				eleStock = eleStock.nextSiblingElement("Stock");
			}
		}
	}

	return true;
}

bool CColorBlockWidget::savePanelInfo( QDomDocument& doc,QDomElement& eleWidget )
{
	if(!CBaseBlockWidget::savePanelInfo(doc,eleWidget))
		return false;

	//当前的板块名称
	if(m_pBlock)
	{
		QDomElement eleBlock = doc.createElement("block");
		eleBlock.appendChild(doc.createTextNode(m_pBlock->getOnly()));
		eleWidget.appendChild(eleBlock);
	}

	{
		//当前的表达式
		QDomElement eleShowType = doc.createElement("ShowType");
		//子节点
		eleShowType.setAttribute("increase",static_cast<int>(m_bShowIncrease));
		eleShowType.setAttribute("turnratio",static_cast<int>(m_bShowTurnRatio));
		eleShowType.setAttribute("volumeratio",static_cast<int>(m_bShowVolumeRatio));
		eleWidget.appendChild(eleShowType);
	}

	{
		//置顶股票
		QDomElement eleTopStocks = doc.createElement("TopStocks");
		foreach(CStockInfoItem* pItem,m_listTopStocks)
		{
			if(pItem->isInstanceOfStock())
			{
				QDomElement eleStock = doc.createElement("Stock");
				eleStock.appendChild(doc.createTextNode(pItem->getOnly()));
				eleTopStocks.appendChild(eleStock);
			}
		}
		eleWidget.appendChild(eleTopStocks);
	}

	return true;
}

void CColorBlockWidget::updateSortMode(bool bSelFirst)
{
	if(bSelFirst)
	{
		m_pSelectedStock = 0;
		showStockIndex = 0;
	}

	//更新显示
	updateColorBlockData();
	return;
}

void CColorBlockWidget::updateTimesH()
{
	//更新当前的横坐标数据，从后向前计算时间
	m_mapTimes = CDataEngine::getTodayTimeMap(Min5);
}

void CColorBlockWidget::setBlock( const QString& block )
{
	clearTmpData();
	CBlockInfoItem* pBlock = CDataEngine::getDataEngine()->getStockBlock(block);
	if(!pBlock)
		return;
	
	m_pBlock = pBlock;

	updateSortMode(true);
	return CBaseWidget::setBlock(block);
}


void CColorBlockWidget::onSetCurrentBlock()
{
	QAction* pAct = reinterpret_cast<QAction*>(sender());
	setBlock(pAct->data().toString());
}

void CColorBlockWidget::setShowType( RShowType _t )
{
	switch(_t)
	{
	case ShowIncrease:
		m_bShowIncrease = !m_bShowIncrease;
		break;
	case ShowTurnRatio:
		m_bShowTurnRatio = !m_bShowTurnRatio;
		break;
	case ShowVolumeRatio:
		m_bShowVolumeRatio = !m_bShowVolumeRatio;
		break;
	}

	updateColorBlockData();
}

void CColorBlockWidget::onSetShowType()
{
	//设置当前的显示类型
	QAction* pAct = reinterpret_cast<QAction*>(sender());
	setShowType(static_cast<RShowType>(pAct->data().toInt()));
	return;
}

void CColorBlockWidget::onSetTopStock()
{
	//弹出设置置顶显示股票的对话框
	QDialog dlg(this);
	QGridLayout layout(&dlg);
	QLabel label1(tr("股票代码"),&dlg);
	QLineEdit editC(&dlg);
	QPushButton btnOk(&dlg);
	dlg.setLayout(&layout);
	layout.addWidget(&label1,0,0,1,1);
	layout.addWidget(&editC,0,1,1,1);
	layout.addWidget(&btnOk,3,0,1,2);
	btnOk.setText(tr("确定"));

	dlg.setWindowTitle(tr("置顶股票设置"));

	connect(&btnOk,SIGNAL(clicked()),&dlg,SLOT(accept()));
	if(QDialog::Accepted != dlg.exec())
		return;

	QString qsCode = editC.text().trimmed();
	CStockInfoItem* pItem = CDataEngine::getDataEngine()->getStockInfoItemByCode(qsCode);
	if(pItem)
	{
		if(!m_listTopStocks.contains(pItem))
		{
			m_listTopStocks.append(pItem);
		}
	}
}

void CColorBlockWidget::onRemoveTopStock()
{
	//弹出设置置顶显示股票的对话框
	QDialog dlg(this);
	QGridLayout layout(&dlg);
	QLabel label1(tr("股票代码"),&dlg);
	QLineEdit editC(&dlg);
	QPushButton btnOk(&dlg);
	dlg.setLayout(&layout);
	layout.addWidget(&label1,0,0,1,1);
	layout.addWidget(&editC,0,1,1,1);
	layout.addWidget(&btnOk,3,0,1,2);
	btnOk.setText(tr("确定"));

	dlg.setWindowTitle(tr("置顶股票设置"));

	connect(&btnOk,SIGNAL(clicked()),&dlg,SLOT(accept()));
	if(QDialog::Accepted != dlg.exec())
		return;

	QString qsCode = editC.text().trimmed();
	CStockInfoItem* pItem = CDataEngine::getDataEngine()->getStockInfoItemByCode(qsCode);
	if(pItem)
	{
		if(m_listTopStocks.contains(pItem))
		{
			m_listTopStocks.removeOne(pItem);
			if(m_pSelectedStock == pItem)
			{
				if(m_listStocks.size()>0)
					clickedStock(m_listStocks.first());
			}
		}
	}
}

void CColorBlockWidget::onSetExpression()
{
	//弹出设置色块大小的对话框，用来设置色块的大小
	/*
	QDialog dlg(this);
	QGridLayout layout(&dlg);
	QLabel label1(tr("颜色"),&dlg);
	QLabel label2(tr("高度"),&dlg);
	QLabel label3(tr("宽度"),&dlg);
	QLineEdit editC(&dlg);
	QLineEdit editH(&dlg);
	QLineEdit editW(&dlg);
	QPushButton btnOk(&dlg);
	dlg.setLayout(&layout);
	layout.addWidget(&label1,0,0,1,1);
	layout.addWidget(&label2,1,0,1,1);
	layout.addWidget(&label3,2,0,1,1);
	layout.addWidget(&editC,0,1,1,1);
	layout.addWidget(&editH,1,1,1,1);
	layout.addWidget(&editW,2,1,1,1);
	layout.addWidget(&btnOk,3,0,1,2);
	btnOk.setText(tr("确定"));
	editC.setText(m_qsExpColor);
	editH.setText(m_qsExpHeight);
	editW.setText(m_qsExpWidth);

	dlg.setWindowTitle(tr("表达式设置"));

	connect(&btnOk,SIGNAL(clicked()),&dlg,SLOT(accept()));
	if(QDialog::Accepted != dlg.exec())
		return;

	m_qsExpColor = editC.text().trimmed();
	m_qsExpHeight = editH.text().trimmed();
	m_qsExpWidth = editW.text().trimmed();
	*/
}

void CColorBlockWidget::updateColorBlockData()
{
	m_listStocks.clear();
	if(m_pBlock)
	{
		QList<CStockInfoItem*> list = m_pBlock->getStockList();
		foreach (CStockInfoItem* pItem,list)
		{
			if(isMatchAbnomal(pItem))
				m_listStocks.append(pItem);
		}
	}

	//进行重新排序
	if(m_sort<=SortByCode)
	{
		QMultiMap<QString,CStockInfoItem*> mapSort;
		foreach(CStockInfoItem* pItem,m_listStocks)
		{
			if(m_sort == SortByCode)
				mapSort.insert(pItem->getOnly(),pItem);
		}
		if(m_sortOrder==Qt::AscendingOrder)
			m_listStocks = mapSort.values();
		else
		{
			QList<CStockInfoItem*> list;
			QMultiMap<QString,CStockInfoItem*>::iterator iter = mapSort.begin();
			while(iter!=mapSort.end())
			{
				list.push_front(iter.value());
				++iter;
			}
			m_listStocks = list;
		}
	}
	else
	{
		QMultiMap<float,CStockInfoItem*> mapSort;
		foreach(CStockInfoItem* pItem,m_listStocks)
		{
			float v = 0.0;
			if(m_sort == SortByIncrease)
				v = _isnan(pItem->getIncrease()) ? 0.0 : pItem->getIncrease();
			else if(m_sort == SortByTurnRatio)
				v = _isnan(pItem->getTurnRatio()) ? 0.0 : pItem->getTurnRatio();
			else if(m_sort == SortByVolumeRatio)
				v = _isnan(pItem->getVolumeRatio()) ? 0.0 : pItem->getVolumeRatio();
			else
				v = _isnan(pItem->getIncrease()) ? 0.0 : pItem->getIncrease();

			mapSort.insert(v,pItem);
		}
		if(m_sortOrder==Qt::AscendingOrder)
			m_listStocks = mapSort.values();
		else
		{
			QList<CStockInfoItem*> list;
			QMultiMap<float,CStockInfoItem*>::iterator iter = mapSort.begin();
			while(iter!=mapSort.end())
			{
				list.push_front(iter.value());
				++iter;
			}
			m_listStocks = list;
		}
	}

	for(int i=0;i<m_listStocks.size();++i)
	{
		CStockInfoItem* pItem = m_listStocks[i];
		m_mapStockIndex[pItem] = i;
		//建立更新机制(目前采用定时刷新，未使用该更新接口)
	}
	if(m_pSelectedStock == 0 && m_listStocks.size()>0)
	{
		clickedStock(m_listStocks.first());
	}


	updateTimesH();			//更新时间轴

	QList<CStockInfoItem*> listShowItems;
	int iClientHeight = this->rect().height();

	//获取当前需要显示的股票列表
	int iIndex = showStockIndex;
	while (iIndex<m_listStocks.size())
	{
		if((iIndex-showStockIndex)*m_iCBHeight<iClientHeight)
		{
			listShowItems.push_back(m_listStocks[iIndex]);
		}
		else
		{
			break;
		}
		++iIndex;
	}

	update();
}

void CColorBlockWidget::clearTmpData()
{
//	foreach(CStockInfoItem* p,m_listStocks)
	{
		//移除所有和 updateStock关联的 信号/槽
//		disconnect(p,SIGNAL(stockItemHistoryChanged(const QString&)),this,SLOT(updateStock(const QString&)));
//		disconnect(p,SIGNAL(stockItemFenBiChanged(const QString&)),this,SLOT(updateStock(const QString&)));
	}
	m_pSelectedStock = 0;
	m_listStocks.clear();

	m_mapStockIndex.clear();
}

void CColorBlockWidget::clickedStock( CStockInfoItem* pItem )
{
	if(pItem == m_pSelectedStock)
		return;
	if(!m_listTopStocks.contains(pItem))
	{
		int iShowCount = m_rtClient.height()/m_iCBHeight;
		int iRow = m_mapStockIndex[pItem];
		if((iRow<showStockIndex)||(iRow>showStockIndex+iShowCount))
		{
			showStockIndex = iRow;
			update(m_rtClient);
		}
	}

	CStockInfoItem* pPreSelectedStock = m_pSelectedStock;
	m_pSelectedStock = pItem;
	update(rectOfStock(pPreSelectedStock));
	update(rectOfStock(m_pSelectedStock));
	if(m_pSelectedStock)
	{
		CMainWindow::getMainWindow()->clickedStock(m_pSelectedStock->getOnly());
	}
}

void CColorBlockWidget::paintEvent( QPaintEvent* )
{
	QPainter p(this);
	QRect rtClient = this->rect();
	if(m_bClearMode)
	{
		m_rtClient = rtClient;
		m_rtBottom = QRect(rtClient.left(),rtClient.bottom()-m_iBottomHeight,rtClient.width(),m_iBottomHeight);

		updateShowTimes(QRect(m_rtBottom.left()+RCB_OFFSET_LEFT,m_rtBottom.top(),
			m_rtBottom.width()-RCB_OFFSET_Y-RCB_OFFSET_LEFT,m_rtBottom.height()),m_iCBWidth);
		drawClient(p,m_rtClient);
	}
	else
	{
		m_rtHeader = QRect(rtClient.left(),rtClient.top(),rtClient.width(),m_iTitleHeight);
		m_rtClient = QRect(rtClient.left(),rtClient.top()+m_iTitleHeight,rtClient.width(),rtClient.height()-m_iTitleHeight-m_iBottomHeight);
		m_rtBottom = QRect(rtClient.left(),rtClient.bottom()-m_iBottomHeight,rtClient.width(),m_iBottomHeight);

		updateShowTimes(QRect(m_rtBottom.left()+RCB_OFFSET_LEFT,m_rtBottom.top(),
			m_rtBottom.width()-RCB_OFFSET_Y-RCB_OFFSET_LEFT,m_rtBottom.height()),m_iCBWidth);
		drawHeader(p,m_rtHeader);
		drawClient(p,m_rtClient);
		drawBottom(p,m_rtBottom);
	}
}

//绘制头部信息
void CColorBlockWidget::drawHeader( QPainter& p,const QRect& rtHeader )
{
	p.fillRect(rtHeader,QColor(0,0,0));
	p.setPen(QColor(255,0,0));
	QRect rtCoord = rtHeader.adjusted(0,0,-1,-1);
	p.drawRect(rtCoord);

	p.setPen(QColor(255,255,255));
	if(!m_pBlock)
		return;

	QString qsText = m_pBlock->getBlockName();
	switch(m_sort)
	{
	case SortByCode:		//按股票代码排序
		qsText += " (代码)";
		break;
	case SortByIncrease:		//按涨幅排序
		qsText += " (涨幅)";
		break;
	case SortByTurnRatio:	//按换手率排序
		qsText += " (换手率)";
		break;
	case SortByVolumeRatio:	//按量比排序
		qsText += " (量比)";
		break;
	default:
		qsText += " (未排序)";
		break;
	}
	
	if(m_sortOrder == Qt::AscendingOrder)
	{
		p.drawText(rtHeader,Qt::AlignLeft|Qt::AlignVCenter,QString("↑%1").arg(qsText));
	}
	else
	{
		p.drawText(rtHeader,Qt::AlignLeft|Qt::AlignVCenter,QString("↓%1").arg(qsText));
	}

	if(rtHeader.width()>250)
	{
		//绘制排序方式的按钮
		m_mapSorts.clear();
		int iSize = m_listSortOp.size();
		int iRight = rtHeader.right()-10;
		int iTop = rtHeader.top();
		for (int i=iSize-1;i>=0;--i)
		{
			QRect rtCircle = QRect(iRight-(iSize-i)*16,iTop+2,12,12);
			m_mapSorts[m_listSortOp[i].value] = rtCircle;
			if(m_sort == m_listSortOp[i].value)
			{
				p.fillRect(rtCircle,QColor(127,127,127));
			}
			else
			{
				p.setPen(QColor(240,240,240));
				p.drawRect(rtCircle);
			}
			p.setPen(QColor(255,255,255));
			p.drawText(rtCircle,m_listSortOp[i].desc.left(1),QTextOption(Qt::AlignCenter));
		}
	}
}

void CColorBlockWidget::drawBottom( QPainter& p,const QRect& rtBottom )
{
	p.fillRect(rtBottom,QColor(0,0,0));

	QRectF rtColors = QRectF(rtBottom.left(),rtBottom.top(),50,rtBottom.height());
	float fColorsWidth = rtColors.width()-5;
	float fColorWidth = fColorsWidth/COLOR_BLOCK_SIZE;
	for(int i=0;i<COLOR_BLOCK_SIZE;++i)
	{
		p.fillRect(QRectF(rtBottom.left()+i*fColorWidth,rtBottom.top(),fColorWidth,rtBottom.height()),
			QColor::fromRgb(CColorManager::getBlockColor(m_qsColorMode,i)));
	}

	//从右向左绘制横坐标
	drawCoordX(p,QRect(rtBottom.left()+RCB_OFFSET_LEFT,rtBottom.top(),
		rtBottom.width()-RCB_OFFSET_Y-RCB_OFFSET_LEFT,rtBottom.height()),m_iCBWidth);
}

void CColorBlockWidget::drawClient( QPainter& p,const QRect& rtClient )
{
	p.fillRect(rtClient,QColor(0,0,0));

	int iCurY = rtClient.top();
	//绘制置顶股票
	foreach(CStockInfoItem* pItem,m_listTopStocks)
	{
		drawStock(p,QRect(rtClient.left(),iCurY,rtClient.width(),m_iCBHeight),pItem);
		iCurY=(iCurY+m_iCBHeight);
		if(iCurY<rtClient.bottom())
			break;
	}

	//绘制待绘制股票
	int iIndex = showStockIndex;
	for(;iCurY<rtClient.bottom();iCurY=(iCurY+m_iCBHeight))
	{
		if(iIndex<0||iIndex>=m_listStocks.size())
			break;

		drawStock(p,QRect(rtClient.left(),iCurY,rtClient.width(),m_iCBHeight),m_listStocks[iIndex]);
		++iIndex;
	}


	if(m_listTopStocks.size()>0)
	{
		//绘制置顶股票周围的边框
		p.setPen(QColor(255,0,0));
		p.drawRect(rtClient.left(),rtClient.top(),rtClient.width(),m_listTopStocks.size()*m_iCBHeight);
	}
}

void CColorBlockWidget::drawStock( QPainter& p,const QRect& rtCB,CStockInfoItem* pItem )
{
	if(pItem == m_pSelectedStock)
	{
		p.fillRect(rtCB,QColor(50,50,50));
	}

	/*
	{
		//绘制左侧的标识信息（代码或者名称）
		p.setPen(QColor(255,255,255));
		QString qsText = pItem->getName();
		if(qsText.isEmpty())
			qsText = pItem->getCode();
		if(rtCB.height()>35)
		{
			//如果单个色块的高度足够的话，则显示两行
			if(m_sort == SortByIncrease)
				qsText += QString("\n%1%").arg(pItem->getIncrease(),0,'f',2);
			else if(m_sort == SortByTurnRatio)
				qsText += QString("\n%1%").arg(pItem->getTurnRatio(),0,'f',2);
			else if(m_sort == SortByVolumeRatio)
				qsText += QString("\n%1%").arg(pItem->getVolumeRatio(),0,'f',2);
		}
		p.drawText(QRect(rtCB.left(),rtCB.top(),RCB_OFFSET_LEFT,m_iCBHeight),Qt::AlignCenter,qsText);
	}
	*/

	//从右向左绘制横坐标
	float fBeginX = rtCB.right()-RCB_OFFSET_Y;
	float fEndX = rtCB.left()+RCB_OFFSET_LEFT;
	float fCBWidth = fBeginX-fEndX;
	if(fCBWidth<0)
		return;

	QList<RStockData*> listDatas = pItem->get5MinList();

	QVector<float> _vColor;
	if(m_bShowIncrease)
		_vColor = getVectorForTime(m_mapTimes,listDatas,ShowIncrease,pItem);
	QVector<float> _vHeight;
	if(m_bShowTurnRatio)
		_vHeight = getVectorForTime(m_mapTimes,listDatas,ShowTurnRatio,pItem);
	QVector<float> _vWidth;
	if(m_bShowVolumeRatio)
		_vWidth = getVectorForTime(m_mapTimes,listDatas,ShowVolumeRatio,pItem);
	//绘制
	drawColocBlock(p,rtCB.top(),_vColor,_vHeight,_vWidth);
}


void CColorBlockWidget::mouseMoveEvent( QMouseEvent* e )
{
	e->accept();
	QPoint ptCur = e->pos();
	if(m_rtClient.contains(ptCur))
	{
		CStockInfoItem* pItem = hitTestStock(ptCur);
		if(pItem)
		{
			clickedStock(pItem);
		}
	}
	return;
	
	if(!((qApp->mouseButtons())&Qt::LeftButton))
	{
		update();
		QToolTip::hideText();
		return CBaseBlockWidget::mouseMoveEvent(e);
	}

	CStockInfoItem* pStock = hitTestStock(e->pos());
	RStockData* item = hitTestCBItem(e->pos());
	if(item == NULL || pStock==0)
	{
		return QToolTip::hideText();
	}
	qDebug()<<QDateTime::fromTime_t(item->tmTime).toString();

	if(!m_rtClient.contains(e->pos()))
		return QToolTip::hideText();

	QString qsTooltip;		//Tips
	QString qsTime = qsTime = QDateTime::fromTime_t(item->tmTime).toString("hh:mm:ss");	
	qsTooltip = QString("股票代码:%1\r\n时间:%2\r\n最新价:%7\r\n最高价:%3\r\n最低价:%4\r\n成交量:%5\r\n成交额:%6")
		.arg(pStock->getCode()).arg(qsTime).arg(item->fHigh).arg(item->fLow)
		.arg(item->fVolume).arg(item->fAmount).arg(item->fClose);

	QToolTip::showText(e->globalPos(),qsTooltip,this);

	return CBaseBlockWidget::mouseMoveEvent(e);
}

void CColorBlockWidget::mousePressEvent( QMouseEvent* e )
{
	e->accept();
	QPoint ptCur = e->pos();
	if(m_rtHeader.contains(ptCur))
	{
		{
			//判断是否属于排序方式按钮
			QMap<int,QRect>::iterator iter = m_mapSorts.begin();
			while(iter!=m_mapSorts.end())
			{
				if((*iter).contains(ptCur))
				{
					setSortMode(static_cast<RSortType>(iter.key()));
					return;
				}
				++iter;
			}
		}


		m_sortOrder = (m_sortOrder==Qt::AscendingOrder) ? Qt::DescendingOrder : Qt::AscendingOrder;
		updateSortMode(true);
		return;

	}
	else if(m_rtClient.contains(ptCur))
	{
		CStockInfoItem* pItem = hitTestStock(ptCur);
		if(pItem)
			clickedStock(pItem);
	}
	else if(m_rtBottom.contains(ptCur))
	{

	}

	return CBaseBlockWidget::mousePressEvent(e);
}

void CColorBlockWidget::wheelEvent( QWheelEvent* e )
{
	int numDegrees = e->delta() / 8;
	int numSteps = numDegrees / 15;
	int iIndex = showStockIndex-numSteps*5;
	if(iIndex<0) {iIndex = 0;}
	if(iIndex>=0&&iIndex<m_listStocks.size())
	{
		e->accept();
		showStockIndex = iIndex;
		updateColorBlockData();
	}
	return CBaseWidget::wheelEvent(e);
}

void CColorBlockWidget::keyPressEvent( QKeyEvent* e )
{
	int iKey = e->key();
	if(Qt::Key_Left == iKey)
	{
		e->accept();
		return;
	}
	else if(Qt::Key_Right == iKey)
	{
		e->accept();
		return;
	}
	else if(Qt::Key_Down == iKey)
	{
		int iCurIndex = m_mapStockIndex[m_pSelectedStock];
		if(m_listStocks.size()>(iCurIndex+1))
		{
			CStockInfoItem* pItem = m_listStocks[iCurIndex+1];
			int iRow = m_mapStockIndex[pItem];
			int iShowCount = m_rtClient.height()/m_iCBHeight;
			if(iShowCount<1)
				return;
			if((iRow-showStockIndex)>=iShowCount)
			{
				showStockIndex = iRow-iShowCount+1;
				updateColorBlockData();
			}
			clickedStock(m_listStocks[iCurIndex+1]);
		}
		e->accept();
		return;
	}
	else if(Qt::Key_Up == iKey)
	{
		int iCurIndex = m_mapStockIndex[m_pSelectedStock];
		if(iCurIndex>0)
		{
			CStockInfoItem* pItem = m_listStocks[iCurIndex-1];
			int iRow = m_mapStockIndex[pItem];
			if(iRow<showStockIndex)
			{
				showStockIndex = iRow;
				updateColorBlockData();
			}
			clickedStock(pItem);
		}
		e->accept();
		return;
	}
	else if(Qt::Key_PageDown == iKey)
	{
		int iShowCount = m_rtClient.height()/m_iCBHeight;
		if(iShowCount<1)
			return;
		if((showStockIndex+iShowCount)<m_listStocks.size())
		{
			showStockIndex = showStockIndex+iShowCount;
			updateColorBlockData();
		}
		e->accept();
		return;
	}
	else if(Qt::Key_PageUp == iKey)
	{
		int iShowCount = m_rtClient.height()/m_iCBHeight;
		if(iShowCount<1)
			return;
		showStockIndex = (showStockIndex-iShowCount)>0 ? (showStockIndex-iShowCount) : 0;
		updateColorBlockData();
		e->accept();
		return;
	}
	else if(Qt::Key_F10 == iKey)
	{
		//F10数据
		if(m_pSelectedStock)
		{
			if(!CDataEngine::getDataEngine()->showF10Data(m_pSelectedStock->getCode()))
			{
				//未打开F10数据 do something
			}
		}
		return;
	}
	else if(Qt::Key_F11 == iKey)
	{
		m_bClearMode = !m_bClearMode;
		update();
		return;
	}

	return CBaseBlockWidget::keyPressEvent(e);
}

void CColorBlockWidget::drawColocBlock( QPainter& p,int iY, QVector<float>& vColor,QVector<float>& vHeight,QVector<float>& vWidth )
{
	float fTimes = 10;			//扩大或缩小倍数

	QMap<time_t,float>::iterator iter = m_mapShowTimes.begin();

	int iMapSize = m_mapTimes.size()-1;
	while(iter!=m_mapShowTimes.end())
	{
		QRectF rtCB = QRectF(iter.value(),iY,m_iCBWidth,m_iCBHeight);
		if(m_mapTimes.contains(iter.key()))
		{
			int iIndex = iMapSize - m_mapTimes[iter.key()];
			if(iIndex>-1)
			{
				float f = vColor.size()>iIndex ? vColor[iIndex] : 1;
				float fH = vHeight.size()>iIndex ? vHeight[iIndex] : 1;
				float fW = vWidth.size()> iIndex ? vWidth[iIndex] : 1;
				if(fH<0)
					fH = 0;
				else if(fH>1)
					fH = 1;

				if(fW<0)
					fW = 0;
				else if(fW>1)
					fW = 1;

				rtCB.adjust(1,1,-1,-1);
				rtCB.setHeight(rtCB.height()*fH);
				rtCB.setWidth(rtCB.width()*fW);
				p.fillRect(rtCB,QColor::fromRgb(CColorManager::getBlockColor(m_qsColorMode,f*fTimes*100)));
			}
		}
		++iter;
	}
	return;
}

QMenu* CColorBlockWidget::getCustomMenu()
{
	CBaseBlockWidget::getCustomMenu();
	{
		//设置所有板块的菜单
		QMenu* pBlockMenu = CMainWindow::getMainWindow()->getBlockMenu(this,BLOCK_CMD_SET);
		pBlockMenu->setTitle(tr("设置当前板块"));
		m_pMenuCustom->addMenu(pBlockMenu);
	}
	{
		//设置当前的显示类型
		QList<QAction*> listAct = m_pMenuShowType->actions();
		foreach(QAction* pAct,listAct)
		{
			pAct->setCheckable(true);
			RShowType _t = static_cast<RShowType>(pAct->data().toInt());
			switch(_t)
			{
			case ShowIncrease:
				pAct->setChecked(m_bShowIncrease);
				break;
			case ShowTurnRatio:
				pAct->setChecked(m_bShowTurnRatio);
				break;
			case ShowVolumeRatio:
				pAct->setChecked(m_bShowVolumeRatio);
				break;
			}
		}
	}

	return m_pMenuCustom;
}


QRect CColorBlockWidget::rectOfStock( CStockInfoItem* pItem )
{
	if(m_listTopStocks.contains(pItem))
	{
		int iRow = m_listTopStocks.indexOf(pItem);
		return QRect(m_rtClient.left(),(m_rtClient.top()+iRow*m_iCBHeight),m_rtClient.width(),m_iCBHeight);
	}
	else if(m_mapStockIndex.contains(pItem))
	{
		int iRow = m_mapStockIndex[pItem];
		iRow += m_listTopStocks.size();
		return QRect(m_rtClient.left(),(m_rtClient.top()+(iRow-showStockIndex)*m_iCBHeight),m_rtClient.width(),m_iCBHeight);
	}

	return QRect();
}

CStockInfoItem* CColorBlockWidget::hitTestStock( const QPoint& ptPoint ) const
{
	int iRow = (ptPoint.y()-m_rtClient.top())/m_iCBHeight;
	if(iRow>-1&&iRow<m_listTopStocks.size())
	{
		//置顶股票
		return m_listTopStocks[iRow];
	}
	iRow = iRow + showStockIndex - m_listTopStocks.size();
	if(iRow<0||iRow>=m_listStocks.size())
		return 0;

	return m_listStocks[iRow];
}

RStockData* CColorBlockWidget::hitTestCBItem( const QPoint& ptPoint ) const
{
	CStockInfoItem* pItem = hitTestStock(ptPoint);

	RStockData* pData = NULL;
	if(pItem)
	{
		int iIndex = (m_rtClient.right() - ptPoint.x())/m_iCBWidth;
		QMap<time_t,int>::iterator iter = m_mapTimes.end();
		while(iter!=m_mapTimes.begin())
		{
			--iter;

			if(iIndex==iter.value())
			{
				pData = pItem->get5MinData(iter.key()/300*300+299);
				if(pData)
				{
					break;
				}
			}
			else if(iIndex<iter.value())
				break;
		}
	}
	return pData;
}

void CColorBlockWidget::getKeyWizData( const QString& keyword,QList<KeyWizData*>& listRet )
{
	foreach(CStockInfoItem* pItem,m_listStocks)
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

	return CBaseBlockWidget::getKeyWizData(keyword,listRet);
}

void CColorBlockWidget::keyWizEntered( KeyWizData* pData )
{
	if(pData->cmd == CKeyWizard::CmdStock)
	{
		clickedStock(reinterpret_cast<CStockInfoItem*>(pData->arg));
		return;
	}

	return CBaseBlockWidget::keyWizEntered(pData);
}

void CColorBlockWidget::onBlockClicked( CBlockInfoItem* pBlock,int iCmd )
{
	if(iCmd == BLOCK_CMD_SET)
	{
		setBlock(pBlock->getOnly());
	}
}

