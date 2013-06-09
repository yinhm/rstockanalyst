#include "StdAfx.h"
#include "ColorBlockWidget.h"
#include "DataEngine.h"
#include "ColorManager.h"
#include "BlockInfoItem.h"
#include "RLuaEx.h"
#include "KeyWizard.h"

#define	RCB_OFFSET_Y	2
#define RCB_OFFSET_LEFT	50

void FreeRStockInfoMap(QMap<time_t,RStockData*>* pMap)
{
	QMap<time_t,RStockData*>::iterator iter = pMap->begin();
	QMap<time_t,RStockData*>::iterator iterEnd = pMap->end();
	while(iter!=iterEnd)
	{
		if((*iter)!=NULL)
		{
			delete (*iter);
		}
		++iter;
	}
}


CColorBlockWidget::CColorBlockWidget( CBaseWidget* parent /*= 0*/ )
	: CBaseBlockWidget(parent,WidgetSColorBlock)
	, m_pMenuBlockList(0)
	, m_iTitleHeight(16)
	, m_iBottomHeight(16)
	, showStockIndex(0)
	, m_pSelectedStock(0)
	, m_pBlock(0)
	, m_qsExpColor("(CLOSE-REF(CLOSE,1))/CLOSE")
	, m_qsExpHeight("CLOSE")
	, m_qsExpWidth("CLOSE")
{
	//设置当前的表达式
	m_pMenuCustom->addAction(tr("设置当前的表达式"),
		this,SLOT(onSetExpression()));

	//设置当前显示的板块
	m_pMenuBlockList = m_pMenuCustom->addMenu(tr("设置当前板块"));

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

	updateData();
	//当前的板块名称
	QDomElement eleBlock = eleWidget.firstChildElement("block");
	if(eleBlock.isElement())
	{
		setBlock(eleBlock.text());
	}

	//当前的表达式
	QDomElement eleExps = eleWidget.firstChildElement("exps");
	if(eleExps.isElement())
	{
		//子节点
		QDomElement eleColor = eleExps.firstChildElement("color");
		if(eleColor.isElement())
			m_qsExpColor = eleColor.text();
		QDomElement eleHeight = eleExps.firstChildElement("height");
		if(eleHeight.isElement())
			m_qsExpHeight = eleHeight.text();
		QDomElement eleWidth = eleExps.firstChildElement("width");
		if(eleWidth.isElement())
			m_qsExpWidth = eleWidth.text();
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

	//当前的表达式
	QDomElement eleExps = doc.createElement("exps");
	{
		//子节点
		QDomElement eleColor = doc.createElement("color");
		QDomElement eleHeight = doc.createElement("height");
		QDomElement eleWidth = doc.createElement("width");
		eleColor.appendChild(doc.createTextNode(m_qsExpColor));
		eleHeight.appendChild(doc.createTextNode(m_qsExpHeight));
		eleWidth.appendChild(doc.createTextNode(m_qsExpWidth));
		eleExps.appendChild(eleColor);
		eleExps.appendChild(eleHeight);
		eleExps.appendChild(eleWidth);
	}
	eleWidget.appendChild(eleExps);

	return true;
}

void CColorBlockWidget::updateData()
{
	updateSortMode(false);
	if(m_typeCircle<Day)
	{
		//分笔数据1秒中更新一次
		m_timerUpdateUI.stop();
		m_timerUpdateUI.start(1000);
	}
	else
	{
		//日线数据10秒中更新一次
		m_timerUpdateUI.stop();
		m_timerUpdateUI.start(10000);
	}
	return/* CBaseBlockWidget::updateData()*/;
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

void CColorBlockWidget::setBlock( const QString& block )
{
	clearTmpData();
	CBlockInfoItem* pBlock = CDataEngine::getDataEngine()->getStockBlock(block);
	if(!pBlock)
		return;
	
	m_pBlock = pBlock;

	m_listStocks = pBlock->getStockList();
	updateSortMode(true);
	return CBaseWidget::setBlock(block);
}

void CColorBlockWidget::setCircle( RStockCircle _c )
{
	CBlockInfoItem* pBlock = m_pBlock;
	clearTmpData();

	m_typeCircle = _c;

	if(!pBlock)
		return;
	m_pBlock = pBlock;
	m_listStocks = pBlock->getStockList();
	updateSortMode(true);
}


void CColorBlockWidget::onSetCurrentBlock()
{
	QAction* pAct = reinterpret_cast<QAction*>(sender());
	setBlock(pAct->data().toString());
}

void CColorBlockWidget::onSetExpression()
{
	//弹出设置色块大小的对话框，用来设置色块的大小
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
}

void CColorBlockWidget::updateColorBlockData()
{
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

	//从map里删除不需要显示的股票
	QMap<CStockInfoItem*,QMap<time_t,RStockData*>*>::iterator iter = mapStockColorBlocks.begin();
	while (iter!=mapStockColorBlocks.end())
	{
		FreeRStockInfoMap(iter.value());
		++iter;
	}
	mapStockColorBlocks.clear();

	
	//将需要显示而map中没有的股票加入到map中
	foreach(CStockInfoItem* p,listShowItems)
	{
		mapStockColorBlocks[p] = getColorBlockMap(p);
	}

	update();
}

void CColorBlockWidget::updateShowMap()
{
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

	//将需要显示而map中没有的股票加入到map中
	foreach(CStockInfoItem* p,listShowItems)
	{
		if(!mapStockColorBlocks.contains(p))
			mapStockColorBlocks[p] = getColorBlockMap(p);
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

	QMap<CStockInfoItem*,QMap<time_t,RStockData*>*>::iterator iter = mapStockColorBlocks.begin();
	while(iter!=mapStockColorBlocks.end())
	{
		FreeRStockInfoMap(*iter);
		delete iter.value();
		++iter;
	}

	mapStockColorBlocks.clear();
	m_mapStockIndex.clear();
}

void CColorBlockWidget::clickedStock( CStockInfoItem* pItem )
{
	if(pItem == m_pSelectedStock)
		return;
	int iShowCount = m_rtClient.height()/m_iCBHeight;
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
}

void CColorBlockWidget::paintEvent( QPaintEvent* )
{
	QPainter p(this);
	QRect rtClient = this->rect();
	m_rtHeader = QRect(rtClient.left(),rtClient.top(),rtClient.width(),m_iTitleHeight);
	m_rtClient = QRect(rtClient.left(),rtClient.top()+m_iTitleHeight,rtClient.width(),rtClient.height()-m_iTitleHeight-m_iBottomHeight);
	m_rtBottom = QRect(rtClient.left(),rtClient.bottom()-m_iBottomHeight,rtClient.width(),m_iBottomHeight);

	updateShowTimes(QRect(m_rtBottom.left()+RCB_OFFSET_LEFT,m_rtBottom.top(),
		m_rtBottom.width()-RCB_OFFSET_Y-RCB_OFFSET_LEFT,m_rtBottom.height()),m_iCBWidth);

	drawHeader(p,m_rtHeader);
	drawClient(p,m_rtClient);
	drawBottom(p,m_rtBottom);
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

	int iSize = m_listCircle.size();
	int iRight = rtHeader.right();
	int iTop = rtHeader.top();
	for (int i=iSize-1;i>=0;--i)
	{
		QRect rtCircle = QRect(iRight-(iSize-i)*16-20,iTop+2,12,12);
		m_mapCircles[m_listCircle[i].value] = rtCircle;
		if(m_typeCircle == m_listCircle[i].value)
		{
			p.fillRect(rtCircle,QColor(127,127,127));
		}
		else
		{
			p.setPen(QColor(240,240,240));
			p.drawRect(rtCircle);
		}
		p.setPen(QColor(255,255,255));
		p.drawText(rtCircle,m_listCircle[i].desc.left(1),QTextOption(Qt::AlignCenter));
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
	int iIndex = showStockIndex;
	for(;iCurY<rtClient.bottom();iCurY=(iCurY+m_iCBHeight))
	{
		if(iIndex<0||iIndex>=m_listStocks.size())
			break;

		drawStock(p,QRect(rtClient.left(),iCurY,rtClient.width(),m_iCBHeight),m_listStocks[iIndex]);
		++iIndex;
	}
}

void CColorBlockWidget::drawStock( QPainter& p,const QRect& rtCB,CStockInfoItem* pItem )
{
	if(pItem == m_pSelectedStock)
	{
		p.fillRect(rtCB,QColor(50,50,50));
	}

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

	//从右向左绘制横坐标
	float fBeginX = rtCB.right()-RCB_OFFSET_Y;
	float fEndX = rtCB.left()+RCB_OFFSET_LEFT;
	float fCBWidth = fBeginX-fEndX;
	if(fCBWidth<0)
		return;
	if(!mapStockColorBlocks.contains(pItem))
		return;

	QMap<time_t,RStockData*>* pMapCBs = mapStockColorBlocks[pItem];
	RCalcInfo calc;
	RDrawInfo draw;
	{
		calc.dwVersion = RSTOCK_VER;
		calc.emCircle = m_typeCircle;
		calc.mapData = pMapCBs;
		calc.mapDataEx = NULL;
		calc.pItem = pItem;

		draw.dwVersion = RSTOCK_VER;
		draw.pPainter = &p;
		draw.rtClient = rtCB;
	}
	{
		lua_pushlightuserdata(m_pL,&calc);
		lua_setglobal(m_pL,"_calc");

		lua_pushlightuserdata(m_pL,&draw);
		lua_setglobal(m_pL,"_draw");

		lua_getglobal(m_pL,"InitValues");
		lua_call(m_pL,0,0);
	}

	{
		QVector<float> _vColor,_vHeight,_vWidth;

		//获取颜色值
		luaL_dostring(m_pL,QString("return %1").arg(m_qsExpColor).toStdString().c_str());
		RLuaEx::LuaRetArray(m_pL,_vColor);

		//获取高度值
		luaL_dostring(m_pL,QString("return %1").arg(m_qsExpHeight).toStdString().c_str());
		RLuaEx::LuaRetArray(m_pL,_vHeight);

		//获取宽度值
		luaL_dostring(m_pL,QString("return %1").arg(m_qsExpWidth).toStdString().c_str());
		RLuaEx::LuaRetArray(m_pL,_vWidth);

		//绘制
		drawColocBlock(p,rtCB.top(),_vColor,_vHeight,_vWidth);
	}
}


void CColorBlockWidget::mouseMoveEvent( QMouseEvent* e )
{
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

	if(!m_rtClient.contains(e->pos()))
		return QToolTip::hideText();

	QString qsTooltip;		//Tips
	QString qsTime;
	if(m_typeCircle<Day)
	{
		qsTime = QDateTime::fromTime_t(item->tmTime).toString("hh:mm:ss");
	}
	else
	{
		QDate dtTmp = QDateTime::fromTime_t(item->tmTime).date();
		if(m_typeCircle == Week)
			qsTime = QString("%1 %2").arg(dtTmp.year()).arg(dtTmp.weekNumber());
		else if(m_typeCircle == Month)
			qsTime = dtTmp.toString("yyyy/MM");
		else if(m_typeCircle == Month3)
			qsTime = dtTmp.toString("yyyy/MM");
		else if(m_typeCircle == Year)
			qsTime = dtTmp.toString("yyyy");
		else
			qsTime = dtTmp.toString("yyyy/MM/dd");
	}
	
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
		QMap<int,QRect>::iterator iter = m_mapCircles.begin();
		while(iter!=m_mapCircles.end())
		{
			if((*iter).contains(ptCur))
			{
				setCircle(static_cast<RStockCircle>(iter.key()));
				break;
			}
			++iter;
		}

		if(iter==m_mapCircles.end())
		{
			m_sortOrder = (m_sortOrder==Qt::AscendingOrder) ? Qt::DescendingOrder : Qt::AscendingOrder;
			updateSortMode(true);
		}
	}
	else if(m_rtClient.contains(ptCur))
	{
		int iCurIndex = showStockIndex+(ptCur.y()-m_rtClient.top())/m_iCBHeight;
		if(iCurIndex>=0&&iCurIndex<m_listStocks.size())
		{
			clickedStock(m_listStocks[iCurIndex]);
		}
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
		updateShowMap();
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
				updateShowMap();
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
				updateShowMap();
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
			updateShowMap();
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
		updateShowMap();
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
	}

	return CBaseBlockWidget::keyPressEvent(e);
}

void CColorBlockWidget::drawColocBlock( QPainter& p,int iY, QVector<float>& vColor,QVector<float>& vHeight,QVector<float>& vWidth )
{
	float fTimes = 1;			//扩大或缩小倍数
	if(m_typeCircle<=Min60)		//小于小时的周期则扩大十倍
		fTimes = 10;
	else if(m_typeCircle<=Week)	//正常周期
		fTimes = 1;
	else						//大于周线的缩小十倍
		fTimes = static_cast<float>(0.1);

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
				float fH = vHeight.size()>iIndex ? vHeight[iIndex]*fTimes : 1;
				float fW = vWidth.size()> iIndex ? vWidth[iIndex]*fTimes : 1;
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
				p.fillRect(rtCB,QColor::fromRgb(CColorManager::getBlockColor(m_qsColorMode,f*fTimes)));
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
		m_pMenuBlockList->clear();
		QList<CBlockInfoItem*> list = CDataEngine::getDataEngine()->getTopLevelBlocks();
		foreach(const CBlockInfoItem* block,list)
		{
			QAction* pAct = m_pMenuBlockList->addAction(block->getBlockName(),this,SLOT(onSetCurrentBlock()));
			pAct->setData(block->getBlockName());
			if(m_pBlock == block)
			{
				pAct->setCheckable(true);
				pAct->setChecked(true);
			}
		}
	}

	return m_pMenuCustom;
}

QRect CColorBlockWidget::rectOfStock( CStockInfoItem* pItem )
{
	if(m_mapStockIndex.contains(pItem))
	{
		int iRow = m_mapStockIndex[pItem];
		return QRect(m_rtClient.left(),(m_rtClient.top()+(iRow-showStockIndex)*m_iCBHeight),m_rtClient.width(),m_iCBHeight);
	}

	return QRect();
}

CStockInfoItem* CColorBlockWidget::hitTestStock( const QPoint& ptPoint ) const
{
	int iRow = (ptPoint.y()-m_rtClient.top())/m_iCBHeight + showStockIndex;
	if(iRow<0||iRow>=m_listStocks.size())
		return 0;

	return m_listStocks[iRow];
}

RStockData* CColorBlockWidget::hitTestCBItem( const QPoint& ptPoint ) const
{
	CStockInfoItem* pItem = hitTestStock(ptPoint);

	RStockData* pData = NULL;
	if(pItem && mapStockColorBlocks.contains(pItem))
	{
		QMap<time_t,RStockData*>* pMap = mapStockColorBlocks[pItem];
		int iIndex = (m_rtClient.right() - ptPoint.x())/m_iCBWidth;
		QMap<time_t,int>::iterator iter = m_mapTimes.end();
		while(iter!=m_mapTimes.begin())
		{
			--iter;

			if(iIndex==iter.value())
			{
				if(pMap->contains(iter.key()))
				{
					pData = pMap->value(iter.key());
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

