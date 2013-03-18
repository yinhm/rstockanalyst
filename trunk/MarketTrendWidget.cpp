#include "StdAfx.h"
#include "MarketTrendWidget.h"
#include "DataEngine.h"
#include "MainWindow.h"

#define	GetColorByFloat(x)	(((x)==0.0) ? QColor(191,191,191) : (((x)>0.0) ? QColor(255,80,80) : QColor(0,255,255)))


CMarketTrendWidget::CMarketTrendWidget( CBaseWidget* parent /*= 0*/ )
	: CBaseWidget(parent,CBaseWidget::MarketTrend)
	, m_iHeaderHeight(20)
	, m_iStockHeight(16)
	, m_iBottomHeight(18)
	, m_rtHeader(0,0,0,0)
	, m_rtClient(0,0,0,0)
	, m_rtBottom(0,0,0,0)
	, showHeaderIndex(0)
	, showStockIndex(0)
	, showBlockIndex(0)
	, m_pSelectedStock(0)
	, m_iSortColumn(0)
	, m_sortOrder(Qt::AscendingOrder)
{
	m_listHeader << tr("索引") << tr("代码") << tr("名称")
		<< tr("涨幅") << tr("量比") << tr("换手率（仓差）") << tr("前收") << tr("今开")
		<< tr("最高") << tr("最低") << tr("最新") << tr("总手") << tr("总额（持仓）")
		<< tr("现手") << tr("涨速") << tr("涨跌") << tr("振幅") << tr("均价") << tr("市盈率")
		<< tr("流通市值") << tr("外盘") << tr("内盘") << tr("委买量") << tr("委卖量")
		<< tr("委比") << tr("委差");

	/*初始化各个单元的宽度*/
	m_listItemWidth.push_back(30);
	m_listItemWidth.push_back(50);
	while(m_listItemWidth.size()<m_listHeader.size())
	{
		m_listItemWidth.push_back(80);
	}
	m_listItemWidth[5] = 100;

	m_pMenuCustom = new QMenu(tr("市场行情图菜单"));
	m_pMenuCustom->addAction(tr("刷新"),this,SLOT(onRefresh()));
	m_pMenuToBlock = new QMenu(tr("添加到板块"));
	m_pMenuCustom->addMenu(m_pMenuToBlock);
	m_pMenuCustom->addAction(tr("删除选中股"),this,SLOT(onRemoveStock()));
	m_pMenuCustom->addSeparator();

	onRefresh();
//	setMinimumHeight(m_iHeaderHeight+m_iStockHeight+m_iBottomHeight);
//	setMinimumWidth(200);
}

CMarketTrendWidget::~CMarketTrendWidget(void)
{
	m_pSelectedStock = 0;
	clearTmpData();
	delete m_pMenuCustom;
}

bool CMarketTrendWidget::loadPanelInfo( const QDomElement& eleWidget )
{
	if(!CBaseWidget::loadPanelInfo(eleWidget))
		return false;
	
	//当前的板块名称
	QDomElement eleBlock = eleWidget.firstChildElement("block");
	if(eleBlock.isElement())
	{
		clickedBlock(eleBlock.text());
	}

	//设置当前选中的股票
	QDomElement eleStock = eleWidget.firstChildElement("stock");
	if(eleStock.isElement())
	{
		CStockInfoItem* pItem = CDataEngine::getDataEngine()->getStockInfoItem(eleStock.text());
		if(pItem)
			clickedStock(pItem);
	}

	return true;
}

bool CMarketTrendWidget::savePanelInfo( QDomDocument& doc,QDomElement& eleWidget )
{
	if(!CBaseWidget::savePanelInfo(doc,eleWidget))
		return false;

	//当前的板块名称
	QDomElement eleBlock = doc.createElement("block");
	eleBlock.appendChild(doc.createTextNode(m_qsSelectedBlock));
	eleWidget.appendChild(eleBlock);

	//当前股票代码
	if(m_pSelectedStock)
	{
		QDomElement eleStock = doc.createElement("stock");
		eleStock.appendChild(doc.createTextNode(m_pSelectedStock->getCode()));
		eleWidget.appendChild(eleStock);
	}
	
	return true;
}

void CMarketTrendWidget::setStocks( const QList<CStockInfoItem*>& list )
{
	clearTmpData();
	m_listStocks = list;
	if(!list.contains(m_pSelectedStock))
	{
		m_pSelectedStock = 0;
	}
	//showHeaderIndex = 0;
	showStockIndex = 0;
	for(int i=0;i<m_listStocks.size();++i)
	{
		m_mapStockIndex[m_listStocks[i]] = i;
	}
	foreach(CStockInfoItem* pItem,m_listStocks)
	{
		connect(pItem,SIGNAL(stockItemReportChanged(const QString&)),this,SLOT(stockInfoChanged(const QString&)));
		connect(pItem,SIGNAL(stockItemHistoryChanged(const QString&)),this,SLOT(stockInfoChanged(const QString&)));
	}

	if(m_listStocks.size()>0)
	{
		clickedStock(m_listStocks.first());
	}
}

void CMarketTrendWidget::stockInfoChanged( const QString& code )
{
	if(m_iSortColumn>2)
	{
		static QTime tmLast = QTime::currentTime();
		if(tmLast.secsTo(QTime::currentTime())>2)
		{
			resortStocks();
			tmLast = QTime::currentTime();
		}
	}
	CStockInfoItem* pItem = CDataEngine::getDataEngine()->getStockInfoItem(code);
	update(rectOfStock(pItem));
}

void CMarketTrendWidget::onRefresh()
{
	m_listBlocks.clear();
	QList<CBlockInfoItem*> listBlocks = CDataEngine::getDataEngine()->getStockBlocks();
	if(listBlocks.size()>0)
	{
		foreach(CBlockInfoItem* b,listBlocks)
		{
			m_listBlocks.push_back(QPair<QString,QRect>(b->getBlockName(),QRect()));
		}
		updateBlockRect();
		if(m_qsSelectedBlock.isEmpty())
			clickedBlock(listBlocks.first()->getBlockName());
		else
			clickedBlock(m_qsSelectedBlock);
	}
}

void CMarketTrendWidget::onAddToBlock()
{
	if(!m_pSelectedStock)
		return;
	QAction* pAct = reinterpret_cast<QAction*>(sender());
	if(!pAct)
		return;
	QString block = pAct->data().toString();
	if(block.isEmpty())
		return;
	CBlockInfoItem* pBlock = CDataEngine::getDataEngine()->getStockBlock(block);
	if(pBlock)
	{
		pBlock->appendStocks(QStringList()<<m_pSelectedStock->getCode());
	}
}

void CMarketTrendWidget::onAddToNewBlock()
{
	if(!m_pSelectedStock)
		return;
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

	QString block = edit.text();
	if(CDataEngine::getDataEngine()->isHadBlock(block))
		return;
	CBlockInfoItem* pBlock = CDataEngine::getDataEngine()->getStockBlock(block);
	if(pBlock)
	{
		pBlock->appendStocks(QStringList()<<m_pSelectedStock->getCode());
	}
}

void CMarketTrendWidget::onRemoveStock()
{
	if(m_pSelectedStock)
	{
		CBlockInfoItem* pBlock = CDataEngine::getDataEngine()->getStockBlock(m_qsSelectedBlock);
		if(pBlock)
		{
			if(pBlock->removeStocks(QStringList()<<m_pSelectedStock->getCode()))
				clickedBlock(m_qsSelectedBlock);
		}
	}
}

void CMarketTrendWidget::clearTmpData()
{
	//disconnect(this,SLOT(stockInfoChanged(const QString&)));
	foreach(CStockInfoItem* pItem,m_listStocks)
	{
		disconnect(pItem,SIGNAL(stockItemReportChanged(const QString&)),this,SLOT(stockInfoChanged(const QString&)));
		disconnect(pItem,SIGNAL(stockItemHistoryChanged(const QString&)),this,SLOT(stockInfoChanged(const QString&)));
	}
	m_listStocks.clear();
	m_mapStockIndex.clear();
}

void CMarketTrendWidget::clickedHeader( int column )
{
	if(column==0)
	{
		m_iSortColumn = 0;
		return;
	}
	if(column == m_iSortColumn)
	{
		m_sortOrder = (m_sortOrder==Qt::AscendingOrder) ? Qt::DescendingOrder : Qt::AscendingOrder;
	}
	else
	{
		m_iSortColumn = column;
		m_sortOrder = Qt::AscendingOrder;
	}
	resortStocks();
}

void CMarketTrendWidget::clickedStock( CStockInfoItem* pItem )
{
	if(pItem == m_pSelectedStock)
		return;
	int iShowCount = m_rtClient.height()/m_iStockHeight;
	int iRow = m_mapStockIndex[pItem];
	if((iRow<showStockIndex)||(iRow>showStockIndex+iShowCount))
	{
		showStockIndex = iRow;
		update(m_rtClient);
	}

	CStockInfoItem* pPreSelectedStock = m_pSelectedStock;
	m_pSelectedStock = pItem;
	update(rectOfStock(pPreSelectedStock));
	update(rectOfStock(m_pSelectedStock));
	CMainWindow::getMainWindow()->clickedStock(pItem->getCode());
}

void CMarketTrendWidget::offsetShowHeaderIndex( int offset )
{
	int iIndex = showHeaderIndex+offset;
	if(iIndex>=0&&iIndex<m_listHeader.size()-3)
	{
		showHeaderIndex = iIndex;
		update();
	}
}

void CMarketTrendWidget::clickedBlock( const QString& block )
{
	CBlockInfoItem* pBlock = CDataEngine::getDataEngine()->getStockBlock(block);
	if(!pBlock)
		return;
	if(m_qsSelectedBlock == block)
	{
		setStocks(pBlock->getStockList());
		CMainWindow::getMainWindow()->clickedBlock(block);
		resortStocks();
		update();
		return;
	}

	{
		setStocks(pBlock->getStockList());
		CMainWindow::getMainWindow()->clickedBlock(block);
		m_qsSelectedBlock = block;
		{
			//设置排序方式
//			m_sortOrder = (m_sortOrder==Qt::AscendingOrder) ? Qt::DescendingOrder : Qt::AscendingOrder;
			clickedHeader(0);
		}
	}
	update();
}

void CMarketTrendWidget::paintEvent( QPaintEvent* )
{
	QPainter p(this);
	drawHeaders(p);
	drawStocks(p);
	drawBottom(p);
}

void CMarketTrendWidget::resizeEvent( QResizeEvent* e)
{
	updateDrawRect();
	return CBaseWidget::resizeEvent(e);
}

void CMarketTrendWidget::mousePressEvent( QMouseEvent* e )
{
	e->accept();
	QPoint ptCur = e->pos();
	if(m_rtHeader.contains(ptCur))
	{
		int iCurX = m_rtHeader.left();
		int iCurIndex = 0;
		for (;iCurIndex<3;++iCurIndex)
		{
			iCurX = iCurX+m_listItemWidth[iCurIndex];
			if(iCurX>ptCur.x())
				break;
		}
		if(iCurX>ptCur.x())
		{
			clickedHeader(iCurIndex);
			return;
		}
		iCurIndex = iCurIndex+showHeaderIndex;
		for (;iCurIndex<m_listItemWidth.size();++iCurIndex)
		{
			iCurX = iCurX+m_listItemWidth[iCurIndex];
			if(iCurX>ptCur.x())
				break;
		}
		if(iCurX>ptCur.x())
		{
			clickedHeader(iCurIndex);
			return;
		}
	}
	else if(m_rtClient.contains(ptCur))
	{
		int iCurIndex = showStockIndex+(ptCur.y()-m_rtClient.top())/m_iStockHeight;
		if(iCurIndex>=0&&iCurIndex<m_listStocks.size())
		{
			clickedStock(m_listStocks[iCurIndex]);
		}
	}
	else if(m_rtBottom.contains(ptCur))
	{
		if(m_rtPreIndex.contains(ptCur))
		{
			offsetShowHeaderIndex(-1);
		}
		else if(m_rtNextIndex.contains(ptCur))
		{
			offsetShowHeaderIndex(1);
		}
		else
		{
			QList<QPair<QString,QRect>>::iterator iter = m_listBlocks.begin();
			while(iter!=m_listBlocks.end())
			{
				if(iter->second.contains(ptCur))
				{
					clickedBlock(iter->first);
					break;
				}
				++iter;
			}
		}
	}
}

void CMarketTrendWidget::wheelEvent( QWheelEvent* e )
{
	int numDegrees = e->delta() / 8;
	int numSteps = numDegrees / 15;
	int iIndex = showStockIndex-numSteps*5;
	if(iIndex<0) {iIndex = 0;}
	if(iIndex>=0&&iIndex<m_listStocks.size())
	{
		e->accept();
		showStockIndex = iIndex;
		update();
	}
	return CBaseWidget::wheelEvent(e);
}

void CMarketTrendWidget::keyPressEvent( QKeyEvent* e )
{
	if(Qt::Key_Left == e->key())
	{
		e->accept();
		return offsetShowHeaderIndex(-1);
	}
	else if(Qt::Key_Right == e->key())
	{
		e->accept();
		return offsetShowHeaderIndex(1);
	}
	else if(Qt::Key_Down == e->key())
	{
		int iCurIndex = m_mapStockIndex[m_pSelectedStock];
		if(m_listStocks.size()>(iCurIndex+1))
		{
			CStockInfoItem* pItem = m_listStocks[iCurIndex+1];
			int iRow = m_mapStockIndex[pItem];
			int iShowCount = m_rtClient.height()/m_iStockHeight;
			if(iShowCount<1)
				return;
			if((iRow-showStockIndex)>=iShowCount)
			{
				showStockIndex = iRow-iShowCount+1;
				update(m_rtClient);
			}
			clickedStock(m_listStocks[iCurIndex+1]);
		}
		e->accept();
	}
	else if(Qt::Key_Up == e->key())
	{
		int iCurIndex = m_mapStockIndex[m_pSelectedStock];
		if(iCurIndex>0)
		{
			CStockInfoItem* pItem = m_listStocks[iCurIndex-1];
			int iRow = m_mapStockIndex[pItem];
			if(iRow<showStockIndex)
			{
				showStockIndex = iRow;
				update(m_rtClient);
			}
			clickedStock(pItem);
		}
		e->accept();
	}
	else if(Qt::Key_PageDown == e->key())
	{
		int iShowCount = m_rtClient.height()/m_iStockHeight;
		if(iShowCount<1)
			return;
		if((showStockIndex+iShowCount)<m_listStocks.size())
		{
			showStockIndex = showStockIndex+iShowCount;
			update(m_rtClient);
		}
		e->accept();
	}
	else if(Qt::Key_PageUp == e->key())
	{
		int iShowCount = m_rtClient.height()/m_iStockHeight;
		if(iShowCount<1)
			return;
		showStockIndex = (showStockIndex-iShowCount)>0 ? (showStockIndex-iShowCount) : 0;
		update(m_rtClient);

		e->accept();
	}

	return CBaseWidget::keyPressEvent(e);
}

QMenu* CMarketTrendWidget::getCustomMenu()
{
	update();
	QAction* pAction = m_pMenu->menuAction();
	if(!m_pMenuCustom->actionGeometry(pAction).isValid())
		m_pMenuCustom->addMenu(m_pMenu);

	m_pMenuToBlock->clear();
	QList<CBlockInfoItem*> list = CDataEngine::getDataEngine()->getStockBlocks();
	foreach(CBlockInfoItem* block,list)
	{
		if(block->getBlockName() == m_qsSelectedBlock)
			continue;
		QAction* pAct = m_pMenuToBlock->addAction(block->getBlockName(),this,SLOT(onAddToBlock()));
		pAct->setData(block->getBlockName());
	}
	m_pMenuToBlock->addSeparator();
	m_pMenuToBlock->addAction(tr("新建板块"),this,SLOT(onAddToNewBlock()));

	return m_pMenuCustom;
}

void CMarketTrendWidget::updateDrawRect()
{
	QRect rtClient = this->rect();
	m_rtHeader = QRect(rtClient.topLeft(),QSize(rtClient.width(),m_iHeaderHeight));
	m_rtBottom = QRect(rtClient.left(),rtClient.bottom()-m_iBottomHeight,rtClient.width(),m_iBottomHeight);
	m_rtClient = QRect(m_rtHeader.left(),m_rtHeader.bottom()+2,rtClient.width(),rtClient.height()-m_iBottomHeight-m_iHeaderHeight-2);

	//更新前后键位置
	m_rtNextIndex = QRect(m_rtBottom.right()-m_iBottomHeight,m_rtBottom.top(),m_iBottomHeight,m_iBottomHeight);
	m_rtPreIndex = QRect(m_rtBottom.right()-m_iBottomHeight*2,m_rtBottom.top(),m_iBottomHeight,m_iBottomHeight);
	updateBlockRect();
}

void CMarketTrendWidget::updateBlockRect()
{
	//将隐藏项置空
	for(int i=0;i<showBlockIndex;++i)
	{
		if(i<m_listBlocks.size())
			m_listBlocks[i].second = QRect();
	}
	//计算各个区域的大小
	QFont f;
	QFontMetrics m(f);
	int iCurX = m_rtBottom.left();
	for (int i=showBlockIndex;i<m_listBlocks.size();++i)
	{
		int iWidth = m.width(m_listBlocks[i].first) + 16;
		QRect rtEntity = QRect(iCurX,m_rtBottom.top(),iWidth,m_rtBottom.height());
		m_listBlocks[i].second = rtEntity;

		iCurX = iCurX+iWidth;
	}
}

void CMarketTrendWidget::resortStocks()
{
	if(m_iSortColumn == 1||m_iSortColumn ==2)
	{
		QMultiMap<QString,CStockInfoItem*> mapSort;
		foreach(CStockInfoItem* pItem,m_listStocks)
		{
			mapSort.insert(dataOfDisplay(pItem,m_iSortColumn),pItem);
		}

		if(m_sortOrder==Qt::AscendingOrder)
			setStocks(mapSort.values());
		else
		{
			QList<CStockInfoItem*> list;
			QMultiMap<QString,CStockInfoItem*>::iterator iter = mapSort.begin();
			while(iter!=mapSort.end())
			{
				list.push_front(iter.value());
				++iter;
			}
			setStocks(list);
		}
	}
	else
	{
		QMultiMap<float,CStockInfoItem*> mapSort;
		foreach(CStockInfoItem* pItem,m_listStocks)
		{
			QString qsData = dataOfDisplay(pItem,m_iSortColumn);
			qsData = qsData.replace("%","");
			mapSort.insert(qsData.toFloat(),pItem);
		}
		if(m_sortOrder==Qt::AscendingOrder)
			setStocks(mapSort.values());
		else
		{
			QList<CStockInfoItem*> list;
			QMultiMap<float,CStockInfoItem*>::iterator iter = mapSort.begin();
			while(iter!=mapSort.end())
			{
				list.push_front(iter.value());
				++iter;
			}
			setStocks(list);
		}
	}
	update(m_rtHeader);
	update(m_rtClient);
}

QString CMarketTrendWidget::dataOfDisplay( CStockInfoItem* itemData,int column )
{
	switch(column)
	{
	case 0:
		{
			//索引号
			if(m_mapStockIndex.contains(itemData))
				return QString("%1").arg(m_mapStockIndex[itemData]+1);
			return QString("Error");
		}
		break;
	case 1:
		{
			//股票代码
			return itemData->getCode();
		}
		break;
	case 2:
		{
			//股票名称
			return itemData->getName();
		}
		break;
	case 3:
		{
			//涨幅
			if(_isnan(itemData->getIncrease()))
				return QString();
			return QString("%1%").arg(itemData->getIncrease(),0,'f',2);
		}
		break;
	case 4:
		{
			//量比
			if(_isnan(itemData->getVolumeRatio()))
				return QString();
			return QString("%1").arg(itemData->getVolumeRatio(),0,'f',2);
		}
		break;
	case 5:
		{
			//换手率
			if(_isnan(itemData->getTurnRatio()))
				return QString();
			return QString("%1%").arg(itemData->getTurnRatio(),0,'f',2);
		}
		break;
	case 6:
		{
			//前收
			if(_isnan(itemData->getLastClose()))
				return QString();

			return QString("%1").arg(itemData->getLastClose(),0,'f',2);
		}
		break;
	case 7:
		{
			//今开
			if(_isnan(itemData->getOpenPrice()))
				return QString();

			return QString("%1").arg(itemData->getOpenPrice(),0,'f',2);
		}
		break;
	case 8:
		{
			//最高
			if(_isnan(itemData->getHighPrice()))
				return QString();

			return QString("%1").arg(itemData->getHighPrice(),0,'f',2);
		}
		break;
	case 9:
		{
			//最低
			if(_isnan(itemData->getLowPrice()))
				return QString();

			return QString("%1").arg(itemData->getLowPrice(),0,'f',2);
		}
		break;
	case 10:
		{
			//最新
			if(_isnan(itemData->getNewPrice()))
				return QString();

			return QString("%1").arg(itemData->getNewPrice(),0,'f',2);
		}
		break;
	case 11:
		{
			//总手
			if(_isnan(itemData->getTotalVolume()))
				return QString();

			return QString("%1").arg(itemData->getTotalVolume(),0,'f',0);
		}
		break;
	case 12:
		{
			//总额（持仓）
			if(_isnan(itemData->getTotalAmount()))
				return QString();

			return QString("%1").arg(itemData->getTotalAmount()/10000,0,'f',0);
		}
		break;
	case 13:
		{
			//现手
			if(_isnan(itemData->getNowVolume()))
				return QString();

			return QString("%1").arg(itemData->getNowVolume(),0,'f',0);
		}
		break;
	case 14:
		{
			//涨速
			if(_isnan(itemData->getIncSpeed()))
				return QString();

			return QString("%1%").arg(itemData->getIncSpeed()*100,0,'f',2);
		}
		break;
	case 15:
		{
			//涨跌
			if(_isnan(itemData->getPriceFluctuate()))
				return QString();

			return QString("%1%").arg(itemData->getPriceFluctuate(),0,'f',2);
		}
		break;
	case 16:
		{
			//振幅
			if(_isnan(itemData->getAmplitude()))
				return QString();

			return QString("%1%").arg(itemData->getAmplitude()*100,0,'f',2);
		}
		break;
	case 17:
		{
			//均价
			if(_isnan(itemData->getAvePrice()))
				return QString();

			return QString("%1").arg(itemData->getAvePrice(),0,'f',2);
		}
		break;
	case 18:
		{
			//市盈率
			if(_isnan(itemData->getPERatio()))
				return QString();
			return QString("%1").arg(itemData->getPERatio(),0,'f',2);
		}
		break;
	case 19:
		{
			//流通市值
			if(_isnan(itemData->getLTSZ()))
				return QString();
			return QString("%1").arg(itemData->getLTSZ(),0,'f',0);
		}
		break;
	case 20:
		{
			//外盘
			if(_isnan(itemData->getSellVOL()))
				return QString();
			return QString("%1").arg(itemData->getSellVOL(),0,'f',0);
		}
		break;
	case 21:
		{
			//内盘
			if(_isnan(itemData->getBuyVOL()))
				return QString();
			return QString("%1").arg(itemData->getBuyVOL(),0,'f',0);
		}
		break;
	case 22:
		{
			//委买量
			if(_isnan(itemData->getBIDVOL()))
				return QString();

			return QString("%1").arg(itemData->getBIDVOL(),0,'f',0);
		}
		break;
	case 23:
		{
			//委卖量
			if(_isnan(itemData->getASKVOL()))
				return QString();

			return QString("%1").arg(itemData->getASKVOL(),0,'f',0);
		}
		break;
	case 24:
		{
			//委比
			if(_isnan(itemData->getCommRatio()))
				return QString();

			return QString("%1%").arg(itemData->getCommRatio(),0,'f',2);
		}
		break;
	case 25:
		{
			//委差
			if(_isnan(itemData->getCommSent()))
				return QString();

			return QString("%1").arg(itemData->getCommSent(),0,'f',0);
		}
		break;
	default:
		{
			return QString("NULL");
		}
		break;
	}
}

QColor CMarketTrendWidget::dataOfColor( CStockInfoItem* itemData,int column )
{
	if(column == 1)
		return QColor(127,255,191);
	else if(column == 2)
		return QColor(255,255,0);
	else if(column == 3 || column == 15)
	{
		//涨幅
		return GetColorByFloat(itemData->getIncrease());
	}
	else if(column == 7)
	{
		//今开
		float f = itemData->getOpenPrice()-itemData->getLastClose();
		return GetColorByFloat(f);
	}
	else if(column == 8)
	{
		//最高
		float f = itemData->getHighPrice()-itemData->getLastClose();
		return GetColorByFloat(f);
	}
	else if(column == 9)
	{
		//最低
		float f = itemData->getLowPrice()-itemData->getLastClose();
		return GetColorByFloat(f);
	}
	else if(column == 10)
	{
		float f = itemData->getNewPrice()-itemData->getLastClose();
		return GetColorByFloat(f);
	}
	else if(column == 17)
	{
		float f = itemData->getAvePrice()-itemData->getLastClose();
		return GetColorByFloat(f);
	}
	else if(column == 20 || column == 22)
		return QColor(255,80,80);
	else if(column == 21 || column == 23)
		return QColor(0,255,255);
	else if(column == 24)
	{
		float f = itemData->getCommRatio();
		return GetColorByFloat(f);
	}
	else if(column == 25)
	{
		float f = itemData->getCommSent();
		return GetColorByFloat(f);
	}

	return QColor(191,191,191);
}

QRect CMarketTrendWidget::rectOfStock( CStockInfoItem* pItem )
{
	if(m_mapStockIndex.contains(pItem))
	{
		int iRow = m_mapStockIndex[pItem];
		return QRect(m_rtClient.left(),(m_rtClient.top()+(iRow-showStockIndex)*m_iStockHeight),m_rtClient.width(),m_iStockHeight);
	}

	return QRect();
}

void CMarketTrendWidget::drawHeaders( QPainter& p )
{
	p.setPen(QColor(255,255,255));
	p.fillRect(m_rtHeader,QColor(0,0,0));
	int iCurX = m_rtHeader.left();
	int iCurIndex = 0;
	for (;iCurIndex<3;++iCurIndex)
	{
		QRect rtItem = QRect(iCurX,m_rtHeader.top(),m_listItemWidth[iCurIndex],m_rtHeader.height());
		p.drawRect(rtItem);
		if((m_iSortColumn == iCurIndex)&&iCurIndex!=0)
		{
			if(m_sortOrder == Qt::AscendingOrder)
			{
				p.drawText(rtItem,Qt::AlignCenter,QString("↑%1").arg(m_listHeader[iCurIndex]));
			}
			else
			{
				p.drawText(rtItem,Qt::AlignCenter,QString("↓%1").arg(m_listHeader[iCurIndex]));
			}
		}
		else
		{
			p.drawText(rtItem,Qt::AlignCenter,m_listHeader[iCurIndex]);
		}
		iCurX = iCurX+m_listItemWidth[iCurIndex];
	}

	iCurIndex = iCurIndex+showHeaderIndex;
	while(iCurX<m_rtHeader.right()&&iCurIndex<m_listHeader.size())
	{
		QRect rtItem = QRect(iCurX,m_rtHeader.top(),m_listItemWidth[iCurIndex],m_rtHeader.height());
		p.drawRect(rtItem);
		if((m_iSortColumn == iCurIndex))
		{
			if(m_sortOrder == Qt::AscendingOrder)
			{
				p.drawText(rtItem,Qt::AlignCenter,QString("↑%1").arg(m_listHeader[iCurIndex]));
			}
			else
			{
				p.drawText(rtItem,Qt::AlignCenter,QString("↓%1").arg(m_listHeader[iCurIndex]));
			}
		}
		else
		{
			p.drawText(rtItem,Qt::AlignCenter,m_listHeader[iCurIndex]);
		}
		iCurX = iCurX+m_listItemWidth[iCurIndex];
		++iCurIndex;
	}
}

void CMarketTrendWidget::drawStocks( QPainter& p )
{
	p.fillRect(m_rtClient,QColor(0,0,0));
	int iCurRow = showStockIndex;
	int iCurY = m_rtClient.top();
	while(iCurY<m_rtClient.bottom()&&iCurRow<m_listStocks.size())
	{
		QRect rtStock = QRect(m_rtClient.left(),iCurY,m_rtClient.width(),m_iStockHeight);
		CStockInfoItem* pStock = m_listStocks[iCurRow];

		if(m_pSelectedStock == pStock)
		{
			p.fillRect(rtStock,QColor(50,50,50));
		}
		drawStock(p,rtStock,pStock);

		++iCurRow;
		iCurY = iCurY + m_iStockHeight;
	}
}

void CMarketTrendWidget::drawBottom( QPainter& p )
{
	p.fillRect(m_rtBottom,QColor(0,0,0));

	p.setPen(QColor(255,255,255));

	QList<QPair<QString,QRect>>::iterator iter = m_listBlocks.begin();
	while(iter!=m_listBlocks.end())
	{
		QRect rtBlock = iter->second;
		if(rtBlock.isValid())
		{
			p.drawRect(rtBlock);
			if(iter->first==m_qsSelectedBlock)
				p.fillRect(rtBlock,QColor(127,127,127));
			p.drawText(rtBlock,Qt::AlignCenter,iter->first);
		}

		++iter;
	}
	drawBottomBtn(p);
}

void CMarketTrendWidget::drawStock( QPainter& p,const QRect& rtStock,CStockInfoItem* pItem )
{
	int iCurX = rtStock.left();
	int iCurIndex = 0;
	for (;iCurIndex<3;++iCurIndex)
	{
		QRect rtItem = QRect(iCurX,rtStock.top(),m_listItemWidth[iCurIndex],rtStock.height());
		p.setPen(QColor(127,0,0));
		p.drawRect(rtItem);
		p.setPen(dataOfColor(pItem,iCurIndex));
		p.drawText(rtItem,Qt::AlignCenter,dataOfDisplay(pItem,iCurIndex));
		iCurX = iCurX+m_listItemWidth[iCurIndex];
	}

	iCurIndex = iCurIndex+showHeaderIndex;
	while(iCurX<m_rtHeader.right()&&iCurIndex<m_listHeader.size())
	{
		QRect rtItem = QRect(iCurX,rtStock.top(),m_listItemWidth[iCurIndex],rtStock.height());
		p.setPen(QColor(127,0,0));
		p.drawRect(rtItem);
		p.setPen(dataOfColor(pItem,iCurIndex));
		p.drawText(rtItem,Qt::AlignCenter,dataOfDisplay(pItem,iCurIndex));
		iCurX = iCurX+m_listItemWidth[iCurIndex];
		++iCurIndex;
	}
}

void CMarketTrendWidget::drawBottomBtn( QPainter& p )
{
	if(m_rtPreIndex.isValid())
	{
		QRect rtBtn = m_rtPreIndex.adjusted(1,1,0,-1);
		p.fillRect(rtBtn,QColor(200,200,200));
		QPainterPath path;
		path.moveTo(rtBtn.center().x()-m_iBottomHeight/2+4,rtBtn.center().y());
		path.lineTo(rtBtn.right()-4,rtBtn.top()+4);
		path.lineTo(rtBtn.right()-4,rtBtn.bottom()-4);
		p.fillPath(path,QColor(0,0,0));
	}
	if(m_rtNextIndex.isValid())
	{
		QRect rtBtn = m_rtNextIndex.adjusted(1,1,0,-1);
		p.fillRect(rtBtn,QColor(200,200,200));
		QPainterPath path;
		path.moveTo(rtBtn.center().x()+m_iBottomHeight/2-4,rtBtn.center().y());
		path.lineTo(rtBtn.left()+4,rtBtn.top()+4);
		path.lineTo(rtBtn.left()+4,rtBtn.bottom()-4);
		p.fillPath(path,QColor(0,0,0));
	}
}
