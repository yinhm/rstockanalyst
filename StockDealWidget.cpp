#include "StdAfx.h"
#include "StockDealWidget.h"
#include "DataEngine.h"
#include "ColorManager.h"
#include "KeyWizard.h"
#define VOL_BUY			0		//主动买
#define VOL_SELL		1		//主动卖
#define VOL_ABBUY		2		//撤买
#define VOL_ABSELL		3		//撤卖
#define VOL_ADBUY		4		//增买
#define VOL_ADSELL		5		//增卖
#define VOL_CROSS		6		//对敲

bool getDealKind( const QList<qRcvFenBiData*>& listFenBi, qRcvFenBiData* pLastFenBi,float* fKind)
{
	foreach(qRcvFenBiData* pFenBi,listFenBi)
	{
		float fVOL = 0.0;
		if(pLastFenBi)
		{
			fVOL = pFenBi->fVolume - pLastFenBi->fVolume;

			//判断买入还是卖出
			if(pFenBi->fPrice>pFenBi->fBuyPrice[0])
			{
				//主动卖
				fKind[VOL_SELL] += fVOL;

				{
					//计算对敲单
					float fVinM = 0.0;
					for(int i=0;i<5;++i)
					{
						fVinM += pLastFenBi->fSellVolume[i];
						if(pLastFenBi->fSellPrice[i]>=pFenBi->fPrice)
						{
							fKind[VOL_CROSS] += (fVOL-fVinM)>0 ? (fVOL-fVinM) : 0;
							break;
						}
					}
				}

				{
					//判断 撤卖、增卖
					int iNow=0,iLast=0;
					while (true)
					{
						float fAbs = pFenBi->fSellPrice[iNow] - pLastFenBi->fSellPrice[iLast];
						if(abs(fAbs)<0.00001)
						{
							//相等
							float fV = pFenBi->fSellVolume[iNow]-pLastFenBi->fSellVolume[iLast];
							if(fV>0)
							{
								fKind[VOL_ADSELL] += fV;
							}
							else
							{
								fKind[VOL_ABSELL] -= fV;
							}

							++iNow;
							++iLast;
						}
						else if(fAbs>0.00001)
						{
							//大于
							fKind[VOL_ABSELL] += pLastFenBi->fSellVolume[iLast];

							++iLast;
						}
						else
						{
							//小于
							fKind[VOL_ADSELL] += pFenBi->fSellVolume[iLast];

							++iNow;
						}

						if(iNow>4)
						{
							break;
						}
						else if(iLast>4)
						{
							break;
						}
					}
				}
				{
					//判断 撤买、增买
					int iNow=0,iLast=0;
					while (true)
					{
						float fAbs = pFenBi->fBuyPrice[iNow] - pLastFenBi->fBuyPrice[iLast];
						if(abs(fAbs)<0.00001)
						{
							//相等
							float fV = pFenBi->fBuyVolume[iNow]-pLastFenBi->fBuyVolume[iLast];
							if(fV>0)
							{
								fKind[VOL_ADBUY] += fV;
							}
							else
							{
								fKind[VOL_ABBUY] -= fV;
							}

							++iNow;
							++iLast;
						}
						else if(fAbs<0.00001)
						{
							//大于
							fKind[VOL_ABBUY] += pLastFenBi->fBuyVolume[iLast];

							++iLast;
						}
						else
						{
							//小于
							fKind[VOL_ADBUY] += pFenBi->fBuyVolume[iLast];

							++iNow;
						}

						if(iNow>4)
						{
							break;
						}
						else if(iLast>4)
						{
							break;
						}
					}
				}
			}
			else
			{
				//主动买
				fKind[VOL_BUY] += fVOL;

				{
					//计算对敲单
					float fVinM = 0.0;
					for(int i=0;i<5;++i)
					{
						fVinM += pLastFenBi->fBuyPrice[i];
						if(pLastFenBi->fBuyPrice[i]<=pFenBi->fPrice)
						{
							fKind[VOL_CROSS] += (fVOL-fVinM)>0 ? (fVOL-fVinM) : 0;
							break;
						}
					}
				}

				{
					//判断 撤卖、增卖
					int iNow=0,iLast=0;
					while (true)
					{
						float fAbs = pFenBi->fSellPrice[iNow] - pLastFenBi->fSellPrice[iLast];
						if(abs(fAbs)<0.00001)
						{
							//相等
							float fV = pFenBi->fSellVolume[iNow]-pLastFenBi->fSellVolume[iLast];
							if(fV>0)
							{
								fKind[VOL_ADSELL] += fV;
							}
							else
							{
								fKind[VOL_ABSELL] -= fV;
							}

							++iNow;
							++iLast;
						}
						else if(fAbs>0.00001)
						{
							//大于
							fKind[VOL_ABSELL] += pLastFenBi->fSellVolume[iLast];

							++iLast;
						}
						else
						{
							//小于
							fKind[VOL_ADSELL] += pFenBi->fSellVolume[iLast];

							++iNow;
						}

						if(iNow>4)
						{
							break;
						}
						else if(iLast>4)
						{
							break;
						}
					}
				}
				{
					//判断 撤买、增买
					int iNow=0,iLast=0;
					while (true)
					{
						float fAbs = pFenBi->fBuyPrice[iNow] - pLastFenBi->fBuyPrice[iLast];
						if(abs(fAbs)<0.00001)
						{
							//相等
							float fV = pFenBi->fBuyVolume[iNow]-pLastFenBi->fBuyVolume[iLast];
							if(fV>0)
							{
								fKind[VOL_ADBUY] += fV;
							}
							else
							{
								fKind[VOL_ABBUY] -= fV;
							}

							++iNow;
							++iLast;
						}
						else if(fAbs<0.00001)
						{
							//大于
							fKind[VOL_ABBUY] += pLastFenBi->fBuyVolume[iLast];

							++iLast;
						}
						else
						{
							//小于
							fKind[VOL_ADBUY] += pFenBi->fBuyVolume[iLast];

							++iNow;
						}

						if(iNow>4)
						{
							break;
						}
						else if(iLast>4)
						{
							break;
						}
					}
				}
			}
		}
		else
		{
			fVOL = pFenBi->fVolume;

			//判断买入还是卖出
			if(pFenBi->fPrice>pFenBi->fBuyPrice[0])
			{
				fKind[VOL_SELL] += fVOL;
			}
			else
			{
				fKind[VOL_BUY] += fVOL;
			}
		}
	}

	return true;
}

bool getPriceIncrease( const QList<qRcvFenBiData*>& listFenBi, qRcvFenBiData* pLastFenBi, const float& fLastClose, float* fIncrese)
{
	if(fLastClose<0.0001)
		return false;

	foreach(qRcvFenBiData* pFenBi,listFenBi)
	{
		float fV = 0.0;
		if(pLastFenBi)
			fV = (pFenBi->fVolume-pLastFenBi->fVolume);
		else
			fV = pFenBi->fVolume;

		int iIndex = ((float)(((pFenBi->fPrice-fLastClose)*100.0)/fLastClose) + 0.5);
		if(iIndex<=-10)
		{
			fIncrese[0] += fV;
		}
		else if(iIndex>=10)
		{
			fIncrese[20] += fV;
		}
		else
		{
			fIncrese[iIndex+10] += fV;
		}

		pLastFenBi = pFenBi;
	}

	return true;
}

//获取挂单分布数据
bool getOrderData( const QList<qRcvFenBiData*>& listFenBi, QMap<int,float>& mapBuy,QMap<int,float>& mapSell)
{
	foreach(qRcvFenBiData* pFenBi,listFenBi)
	{
		for (int i=0;i<5;++i)
		{
			int iPriceBuy = (int)((pFenBi->fBuyPrice[i]*100.0)+0.5);
			int iPriceSell = (int)((pFenBi->fSellPrice[i]*100.0)+0.5);
			if(mapBuy.contains(iPriceBuy))
			{
				mapBuy[iPriceBuy]+=pFenBi->fBuyVolume[i];
			}
			else
			{
				mapBuy[iPriceBuy]=pFenBi->fBuyVolume[i];
			}

			if(mapSell.contains(iPriceSell))
			{
				mapSell[iPriceSell]+=pFenBi->fSellVolume[i];
			}
			else
			{
				mapSell[iPriceSell]=pFenBi->fSellVolume[i];
			}
		}
	}
	return true;
}

CStockDealWidget::CStockDealWidget( CBaseWidget* parent /*= 0*/ )
	: CCoordXBaseWidget(parent,WidgetStockDeal)
	, m_iTitleHeight(16)
	, m_iBottomHeight(16)
	, m_pCurStock(0)
	, m_iItemWidth(4)
	, m_iItemHeight(70)
	, m_typeWidget(DealKind)
{
	{
		//初始化显示类型表
		m_listDealType.push_back(RWidgetOpData(DealKind,".kind","类型图"));
		m_listDealType.push_back(RWidgetOpData(DealIncrese,".price","价位图"));
		m_listDealType.push_back(RWidgetOpData(DealOrder,".order","挂单图"));
		m_listDealType.push_back(RWidgetOpData(DealOrder2,".order2","挂单图2"));
	}

	{
		m_pMenuCustom->removeAction(m_pMenuCircle->menuAction());
		//设置当前图的显示类型
		m_pMenuDealType = m_pMenuCustom->addMenu(tr("类型设置"));
		foreach(const RWidgetOpData& _d,m_listDealType)
		{
			m_pMenuDealType->addAction(_d.desc,this,SLOT(onSetDealType()))->setData(_d.value);
		}

		//颜色显示模式菜单
		m_pMenuColorMode = m_pMenuCustom->addMenu("设置颜色模式");
	}

	//初始化颜色表
	setColorMode("");
}


CStockDealWidget::~CStockDealWidget(void)
{
}

void CStockDealWidget::onSetDealType()
{
	//设置当前的显示类型
	QAction* pAct = reinterpret_cast<QAction*>(sender());
	setDealType(static_cast<RDealWidgetType>(pAct->data().toInt()));
}

void CStockDealWidget::onSetColorMode()
{
	QAction* pAct = reinterpret_cast<QAction*>(sender());
	setColorMode(pAct->data().toString());
}

void CStockDealWidget::setStockCode( const QString& code )
{
	m_pCurStock = CDataEngine::getDataEngine()->getStockInfoItem(code);
	updateData();
}

void CStockDealWidget::setDealType( RDealWidgetType _t )
{
	m_typeWidget = _t;
	updateData();
}

void CStockDealWidget::setColorMode( const QString& mode )
{
	m_vColor.clear();
	m_qsColorMode = mode;
	for (int i=0;i<21;++i)
	{
		m_vColor.push_back(CColorManager::getBlockColor(mode,i));
	}
	updateData();
}

void CStockDealWidget::updateData()
{
	//更新当前的横坐标数据，从后向前计算时间
	m_mapTimes = CDataEngine::getTodayTimeMap(Min5);
	update();
}

void CStockDealWidget::mousePressEvent( QMouseEvent* e )
{
	e->accept();
	QPoint ptCur = e->pos();
	if(m_rtTitle.contains(ptCur))
	{
		{
			//判断是否属于周期设置按钮
			QMap<int,QRect>::iterator iter = m_mapDealTypes.begin();
			while(iter!=m_mapDealTypes.end())
			{
				if((*iter).contains(ptCur))
				{
					setDealType(static_cast<RDealWidgetType>(iter.key()));
					return;
				}
				++iter;
			}
		}
	}
}

void CStockDealWidget::paintEvent( QPaintEvent* /*e*/ )
{
	QPainter p(this);
	QRect rtClient = this->rect();

//	p.setPen(QColor(127,0,0));
//	p.drawRect(rtClient);


	m_rtTitle = QRect(rtClient.left(),rtClient.top(),rtClient.width(),m_iTitleHeight);
	m_rtBottom = QRect(rtClient.left(),rtClient.bottom()-m_iBottomHeight,rtClient.width(),m_iBottomHeight);
	m_rtClient = QRect(rtClient.left(),rtClient.top()+m_iTitleHeight,rtClient.width(),rtClient.height()-m_iBottomHeight-m_iTitleHeight);
	drawTitle(p);
	drawBottom(p);
	drawClient(p);
}

QMenu* CStockDealWidget::getCustomMenu()
{
	QAction* pAction = m_pMenu->menuAction();
	if(!m_pMenuCustom->actionGeometry(pAction).isValid())
		m_pMenuCustom->addMenu(m_pMenu);

	{
		//设置当前选中的显示类型
		QList<QAction*> listAct = m_pMenuDealType->actions();
		foreach(QAction* pAct,listAct)
		{
			pAct->setCheckable(true);
			pAct->setChecked(pAct->data().toInt() == m_typeWidget);
		}
	}

	{
		//添加当前所有的支持的颜色模式菜单
		m_pMenuColorMode->clear();
		QStringList listColors = CColorManager::getBlockColorList();
		foreach(const QString& clr,listColors)
		{
			QAction* pAct = m_pMenuColorMode->addAction(clr,this,SLOT(onSetColorMode()));
			pAct->setData(clr);
			pAct->setCheckable(true);
			if(clr == m_qsColorMode)
				pAct->setChecked(true);
		}
	}

	return m_pMenuCustom;
}

void CStockDealWidget::drawTitle( QPainter& p )
{
	QRect rtTitle = m_rtTitle.adjusted(1,1,-1,-1);
	p.setPen(QColor(255,0,0));
	p.drawRect(rtTitle);
	if(m_pCurStock)
	{
		QString qsText = m_pCurStock->getName();
		if(qsText.isEmpty())
			qsText = m_pCurStock->getCode();
		p.setPen(QColor(255,255,255));
		p.drawText(rtTitle,qsText);
	}

	{
		//m_mapDealTypes
		//绘制周期切换按钮
		m_mapDealTypes.clear();
		int iSize = m_listDealType.size();
		int iRight = m_rtTitle.right();
		int iTop = m_rtTitle.top();
		for (int i=iSize-1;i>=0;--i)
		{
			QRect rtDealType = QRect(iRight-(iSize-i)*16-20,iTop+2,12,12);
			m_mapDealTypes[m_listDealType[i].value] = rtDealType;
			if(m_typeWidget == m_listDealType[i].value)
			{
				p.fillRect(rtDealType,QColor(127,127,127));
			}
			else
			{
				p.setPen(QColor(240,240,240));
				p.drawRect(rtDealType);
			}
			p.setPen(QColor(255,255,255));
			p.drawText(rtDealType,m_listDealType[i].desc.left(1),QTextOption(Qt::AlignCenter));
		}
	}
}

void CStockDealWidget::drawClient( QPainter& p )
{
	if(!m_pCurStock)
		return;
	QVector<uint> vColors;
	CColorManager::getBlockColor(m_qsColorMode,vColors);

	int iBeginX = m_rtClient.right();
//	int iEndX = m_rtClient.left();			//全部显示，暂时不用
	int iBeginY = m_rtClient.top();

	qRcvFenBiData* pLastFenBi = 0;
	int iLastIndex = -1;

	QList<qRcvFenBiData*> allFenBi;
	if(m_pCurStock)
	{
		allFenBi = m_pCurStock->getFenBiList();
	}
	QList<qRcvFenBiData*> listFenBi;
	for (int i=allFenBi.size()-1;i>=0;--i)
	{
		qRcvFenBiData* pFenBi = allFenBi[i];
		QMap<time_t,int>::const_iterator iter = m_mapTimes.lowerBound(pFenBi->tmTime);
		if(iter==m_mapTimes.end())
			continue;

		if(iLastIndex>=0)
		{
			if(iLastIndex == iter.value())
			{
				listFenBi.push_front(pFenBi);
			}
			else
			{
				QRect rtBlock = QRect(iBeginX-(iLastIndex+2)*m_iItemWidth,iBeginY,m_iItemWidth,m_iItemHeight);
				//do something;
				switch(m_typeWidget)
				{
				case DealKind:
					{
						float fDealKind[7];
						memset(&fDealKind[0],0,sizeof(float)*7);
						getDealKind(listFenBi,pFenBi,&fDealKind[0]);
						float fTotal = 0.0;
						for(int i=0;i<7;++i)
							fTotal+=fDealKind[i];
						float fY = rtBlock.top();
						float fH = rtBlock.height();
						for(int j=0;j<7;++j)
						{
							float fh = (fDealKind[j]/fTotal)*fH;
							p.fillRect(rtBlock.left(),fY,rtBlock.width(),fh,vColors[j]);
							fY += fh;
						}
					}
					break;
				case DealIncrese:
					{
						float fDealIncrease[21];
						memset(&fDealIncrease[0],0,sizeof(float)*21);
						getPriceIncrease(listFenBi,pFenBi,m_pCurStock->getLastClose(),&fDealIncrease[0]);
						float fTotal = 0.0;
						for(int j=0;j<21;++j)
							fTotal+=fDealIncrease[j];
						if(fTotal>0.0)
						{
							float fY = rtBlock.top();
							float fH = rtBlock.height();
							for(int j=0;j<21;++j)
							{
								float fh = (fDealIncrease[j]/fTotal)*fH;
								p.fillRect(rtBlock.left(),fY,rtBlock.width(),fh,m_vColor[j]);
								fY += fh;
							}
						}
					}
					break;
				case DealOrder:
					{
						QMap<int,float> mapBuy,mapSell;
						getOrderData(listFenBi,mapBuy,mapSell);
						float fTotalBuy = 0.0;
						float fTotalSell = 0.0;
						{
							QMap<int,float>::iterator iter = mapBuy.begin();
							while (iter!=mapBuy.end())
							{
								fTotalBuy+=iter.value();
								++iter;
							}
						}
						{
							QMap<int,float>::iterator iter = mapSell.begin();
							while (iter!=mapSell.end())
							{
								fTotalSell+=iter.value();
								++iter;
							}
						}
						if(fTotalBuy>0.0)
						{
							float fY = rtBlock.top();
							float fH = rtBlock.height()/2;
							
							QMap<int,float>::iterator iter = mapBuy.begin();
							int j = 20;
							while (iter!=mapBuy.end())
							{
								float fh = (iter.value()/fTotalBuy)*fH;
								if(j<10)
									j=10;
								p.fillRect(rtBlock.left(),fY,rtBlock.width(),fh,m_vColor[j]);
								fY += fh;
								--j;
								++iter;
							}
						}

						if(fTotalSell>0.0)
						{
							float fY = rtBlock.center().y();
							float fH = rtBlock.height()/2;

							QMap<int,float>::iterator iter = mapSell.begin();
							int j=9;
							while (iter!=mapSell.end())
							{
								float fh = (iter.value()/fTotalSell)*fH;
								if(j<0)
									j=0;
								p.fillRect(rtBlock.left(),fY,rtBlock.width(),fh,m_vColor[j]);
								fY += fh;
								--j;
								++iter;
							}
						}
					}
					break;
				case DealOrder2:
					{
						QMap<int,float> mapBuy,mapSell;
						getOrderData(listFenBi,mapBuy,mapSell);
						float fTotalBuy = 0.0;
						float fTotalSell = 0.0;
						{
							QMap<int,float>::iterator iter = mapBuy.begin();
							while (iter!=mapBuy.end())
							{
								fTotalBuy+=iter.value();
								++iter;
							}
						}
						{
							QMap<int,float>::iterator iter = mapSell.begin();
							while (iter!=mapSell.end())
							{
								fTotalSell+=iter.value();
								++iter;
							}
						}
						if(fTotalBuy>0.0)
						{
							float fY = rtBlock.top();
							float fH = rtBlock.height()/2;
							float fPer = fH/10.0;
							if(mapBuy.size()<10)
								fPer = fH/mapBuy.size();

							QMap<int,float>::iterator iter = mapBuy.begin();
							int j = 0;
							float fV=0;
							while (iter!=mapBuy.end())
							{
								if(j>8)
								{
									fV += iter.value();
								}
								else
								{
									int iIndex = ((iter.value()/fTotalBuy)*100)+0.5;
									if(iIndex>9)
										iIndex=9;
									p.fillRect(rtBlock.left(),fY,rtBlock.width(),fPer,m_vColor[20-iIndex]);
									fY += fPer;
								}
								++j;
								++iter;
							}
							if(fV>0.1)
							{
								int iIndex = ((fV/fTotalBuy)*100)+0.5;
								if(iIndex>9)
									iIndex=9;
								p.fillRect(rtBlock.left(),fY,rtBlock.width(),fPer,m_vColor[20-iIndex]);
							}
						}

						if(fTotalSell>0.0)
						{
							float fY = rtBlock.center().y();
							float fH = rtBlock.height()/2;
							float fPer = fH/10.0;
							if(mapSell.size()<10)
								fPer = fH/mapSell.size();

							QMap<int,float>::iterator iter = mapSell.begin();
							int j = 0;
							float fV=0;
							while (iter!=mapSell.end())
							{
								if(j>8)
								{
									fV += iter.value();
								}
								else
								{
									int iIndex = ((iter.value()/fTotalSell)*100)+0.5;
									if(iIndex>9)
										iIndex=9;
									p.fillRect(rtBlock.left(),fY,rtBlock.width(),fPer,m_vColor[9-iIndex]);
									fY += fPer;
								}
								++j;
								++iter;
							}
							if(fV>0.1)
							{
								int iIndex = ((fV/fTotalSell)*100)+0.5;
								if(iIndex>9)
									iIndex=9;
								p.fillRect(rtBlock.left(),fY,rtBlock.width(),fPer,m_vColor[9-iIndex]);
							}
						}
					}
					break;
				}


				listFenBi.clear();
				pLastFenBi = pFenBi;
				iLastIndex = iter.value();
				listFenBi.push_front(pFenBi);
			}
		}
		else
		{
			iLastIndex = iter.value();
			listFenBi.push_front(pFenBi);
		}
	}
}

void CStockDealWidget::drawBottom( QPainter& p )
{
	//从右向左绘制横坐标
	int iBeginX = m_rtBottom.right();
	int iEndX = m_rtBottom.left();
	if((iBeginX-iEndX)<0)
		return;

	QList<time_t> listTimes = m_mapTimes.keys();
	int iCurX = iBeginX-m_iItemWidth;
	int iLastX = iBeginX;
	int iCount = listTimes.size()-1;
	if(iCount<0)
		return;

	int iTimeCount = 0;				//只是用来区分时间的颜色（隔开颜色，便于查看）

	while(iCurX>iEndX && iCount>=0)
	{
		time_t tmTime = listTimes[iCount];
		if((iLastX-iCurX)>30)
		{
			p.setPen( iTimeCount%2 ? QColor(255,0,0) : QColor(0,255,255));
			p.drawLine(iCurX,m_rtBottom.top(),iCurX,m_rtBottom.top()+2);
			p.drawText(iCurX-14,m_rtBottom.top()+2,30,m_rtBottom.height()-2,
				Qt::AlignCenter,QDateTime::fromTime_t(tmTime).toString("hh:mm"));

			iLastX = iCurX;
			++iTimeCount;
		}

		--iCount;
		iCurX = iCurX- m_iItemWidth;
	}
	return;
}

bool CStockDealWidget::loadPanelInfo( const QDomElement& eleWidget )
{
	if(!CBaseWidget::loadPanelInfo(eleWidget))
		return false;

	//当前显示的周期
	if(eleWidget.hasAttribute("dealtype"))
	{
		m_typeWidget = static_cast<RDealWidgetType>(eleWidget.attribute("dealtype").toInt());
	}

	//当前的颜色模式
	QDomElement eleColorMode = eleWidget.firstChildElement("color");
	if(eleColorMode.isElement())
	{
		QString qsColor = eleColorMode.text();
		setColorMode(qsColor);
	}

	QDomElement eleCode = eleWidget.firstChildElement("code");
	if(eleCode.isElement())
		setStockCode(eleCode.text());

	return true;
}

bool CStockDealWidget::savePanelInfo( QDomDocument& doc,QDomElement& eleWidget )
{
	if(!CBaseWidget::savePanelInfo(doc,eleWidget))
		return false;

	//显示的周期
	eleWidget.setAttribute("dealtype",m_typeWidget);
	
	//当前的颜色模式
	QDomElement eleColorMode = doc.createElement("color");
	eleColorMode.appendChild(doc.createTextNode(m_qsColorMode));
	eleWidget.appendChild(eleColorMode);

	//当前的股票值
	if(m_pCurStock)
	{
		QDomElement eleCode = doc.createElement("code");
		eleCode.appendChild(doc.createTextNode(m_pCurStock->getOnly()));
		eleWidget.appendChild(eleCode);
	}

	return true;
}

void CStockDealWidget::getKeyWizData( const QString& keyword,QList<KeyWizData*>& listRet )
{
	foreach(const RWidgetOpData& _d,m_listDealType)
	{
		if(_d.key.indexOf(keyword)>-1)
		{
			KeyWizData* pData = new KeyWizData;
			pData->cmd = CKeyWizard::CmdDealType;
			pData->arg = (void*)_d.value;
			pData->desc = _d.desc;
			listRet.push_back(pData);
			if(listRet.size()>20)
				return;
		}
	}

	return CBaseWidget::getKeyWizData(keyword,listRet);
}

void CStockDealWidget::keyWizEntered( KeyWizData* pData )
{
	if(pData->cmd == CKeyWizard::CmdDealType)
	{
		setDealType(static_cast<RDealWidgetType>((int)(pData->arg)));
		return;
	}
	return CBaseWidget::keyWizEntered(pData);
}
