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

bool getDealKind(QList<qRcvFenBiData*> listFenBi, qRcvFenBiData* pLastFenBi,float* fKind)
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


						++iLast;
					}
					else
					{
						//小于

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
			else
			{
				//主动买
				fKind[VOL_BUY] += fVOL;
			}

			//判断撤买、撤卖
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


CStockDealWidget::CStockDealWidget( CBaseWidget* parent /*= 0*/ )
	: CCoordXBaseWidget(parent,WidgetStockDeal)
	, m_iTitleHeight(16)
	, m_iBottomHeight(16)
	, m_pCurStock(0)
	, m_iItemWidth(4)
	, m_iItemHeight(50)
	, m_typeWidget(DealKind)
{
	{
		//初始化显示类型表
		m_listDealType.push_back(RWidgetOpData(DealKind,".kind","类型图"));
		m_listDealType.push_back(RWidgetOpData(DealIncrese,".price","价位图"));
		m_listDealType.push_back(RWidgetOpData(DealOrder,".order","挂单图"));
	}

	{
		m_pMenuCustom->removeAction(m_pMenuCircle->menuAction());
		//设置当前图的显示类型
		m_pMenuDealType = m_pMenuCustom->addMenu(tr("类型设置"));
		foreach(const RWidgetOpData& _d,m_listDealType)
		{
			m_pMenuDealType->addAction(_d.desc,this,SLOT(onSetDealType()))->setData(_d.value);
		}
	}
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

void CStockDealWidget::updateData()
{
	//更新当前的横坐标数据，从后向前计算时间
	m_mapTimes = CDataEngine::getTodayTimeMap(Min5);
	if(m_pCurStock)
	{
		m_listFenbi = m_pCurStock->getFenBiList();
	}
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

	int iBeginX = m_rtClient.right();
	int iEndX = m_rtClient.left();
	int iBeginY = m_rtClient.top();

	qRcvFenBiData* pLastFenBi = 0;
	int iLastIndex = -1;
	QList<qRcvFenBiData*> listFenBi;
	for (int i=m_listFenbi.size()-1;i>=0;--i)
	{
		qRcvFenBiData* pFenBi = m_listFenbi[i];
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
						memset(&fDealKind[0],0,sizeof(float)*6);
						getDealKind(listFenBi,pFenBi,&fDealKind[0]);
						float fTotal = 0.0;
						for(int i=0;i<7;++i)
							fTotal+=fDealKind[i];
						float fY = rtBlock.top();
						float fH = rtBlock.height();
						for(int i=0;i<7;++i)
						{
							float fh = (fDealKind[i]/fTotal)*fH;
							p.fillRect(rtBlock.left(),fY,rtBlock.width(),fh,CColorManager::getCommonColor(i));
							fY += fh;
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
