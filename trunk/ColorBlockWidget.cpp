#include "StdAfx.h"
#include "ColorBlockWidget.h"
#include "DataEngine.h"

CColorBlockWidget::CColorBlockWidget( CBaseWidget* parent /*= 0*/ )
	: CBaseWidget(parent,CBaseWidget::ColorBlock)
	, m_pMenuCustom(0)
	, m_iCBHeight(16)
	, showStockIndex(0)
	, m_pSelectedStock(0)
{
	m_pMenuCustom = new QMenu(tr("色块图菜单"));
}

CColorBlockWidget::~CColorBlockWidget(void)
{
	clearTmpData();
	delete m_pMenuCustom;
}

bool CColorBlockWidget::loadPanelInfo( const QDomElement& eleWidget )
{
	if(!CBaseWidget::loadPanelInfo(eleWidget))
		return false;

	//当前的板块名称
	QDomElement eleBlock = eleWidget.firstChildElement("block");
	if(eleBlock.isElement())
	{
		setBlock(eleBlock.text());
	}

	return true;
}

bool CColorBlockWidget::savePanelInfo( QDomDocument& doc,QDomElement& eleWidget )
{
	if(!CBaseWidget::savePanelInfo(doc,eleWidget))
		return false;


	//当前的板块名称
	QDomElement eleBlock = doc.createElement("block");
	eleBlock.appendChild(doc.createTextNode(m_qsBlock));
	eleWidget.appendChild(eleBlock);


	return true;
}

void CColorBlockWidget::setBlock( const QString& block )
{
	clearTmpData();

	m_listStocks = CDataEngine::getDataEngine()->getStocksByBlock(block);
	showStockIndex = 0;
	for(int i=0;i<m_listStocks.size();++i)
	{
		CStockInfoItem* pItem = m_listStocks[i];

		m_mapStockIndex[pItem] = i;
		//建立更新机制
		connect(pItem,SIGNAL(stockItemHistoryChanged(const QString&)),this,SLOT(updateStock(const QString&)));
		connect(pItem,SIGNAL(stockItemMinuteChanged(const QString&)),this,SLOT(updateStock(const QString&)));
	}

	m_qsBlock = block;

	if(m_listStocks.size()>0)
	{
		clickedStock(m_listStocks.first());
	}

	update();
	return CBaseWidget::setBlock(block);
}

void CColorBlockWidget::updateStock( const QString& code )
{
	
}

void CColorBlockWidget::clearTmpData()
{
	m_pSelectedStock = 0;
	m_listStocks.clear();
	m_mapStockIndex.clear();

	disconnect(this,SLOT(updateStock(const QString&)));
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
	m_rtHeader = QRect(rtClient.left(),rtClient.top(),rtClient.width(),20);
	m_rtClient = QRect(rtClient.left(),rtClient.top()+21,rtClient.width(),rtClient.height()-40);
	m_rtBottom = QRect(rtClient.left(),rtClient.bottom()-20,rtClient.width(),20);

	drawHeader(p,m_rtHeader);
	drawClient(p,m_rtClient);
	drawBottom(p,m_rtBottom);
}

void CColorBlockWidget::mousePressEvent( QMouseEvent* e )
{
	e->accept();
	QPoint ptCur = e->pos();
	if(m_rtHeader.contains(ptCur))
	{

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
}

void CColorBlockWidget::wheelEvent( QWheelEvent* e )
{
	int numDegrees = e->delta() / 8;
	int numSteps = numDegrees / 15;
	int iIndex = showStockIndex-numSteps*5;
	if(iIndex>=0&&iIndex<m_listStocks.size())
	{
		e->accept();
		showStockIndex = iIndex;
		update();
	}
	return CBaseWidget::wheelEvent(e);
}

void CColorBlockWidget::keyPressEvent( QKeyEvent* e )
{
	if(Qt::Key_Left == e->key())
	{
		e->accept();
		return;
	}
	else if(Qt::Key_Right == e->key())
	{
		e->accept();
		return;
	}
	else if(Qt::Key_Down == e->key())
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
		int iShowCount = m_rtClient.height()/m_iCBHeight;
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
		int iShowCount = m_rtClient.height()/m_iCBHeight;
		if(iShowCount<1)
			return;
		showStockIndex = (showStockIndex-iShowCount)>0 ? (showStockIndex-iShowCount) : 0;
		update(m_rtClient);

		e->accept();
	}

	return CBaseWidget::keyPressEvent(e);
}

QMenu* CColorBlockWidget::getCustomMenu()
{
	QAction* pAction = m_pMenu->menuAction();
	if(!m_pMenuCustom->actionGeometry(pAction).isValid())
		m_pMenuCustom->addMenu(m_pMenu);

	return m_pMenuCustom;
}

void CColorBlockWidget::drawHeader( QPainter& p,const QRect& rtHeader )
{
	p.fillRect(rtHeader,QColor(0,0,0));
	p.setPen(QColor(255,0,0));
	p.drawRect(rtHeader);

	p.setPen(QColor(255,255,255));
	p.drawText(rtHeader,Qt::AlignLeft|Qt::AlignVCenter,m_qsBlock);
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

void CColorBlockWidget::drawBottom( QPainter& p,const QRect& rtBottom )
{
	p.fillRect(rtBottom,QColor(0,0,127));
}

void CColorBlockWidget::drawStock( QPainter& p,const QRect& rtCB,CStockInfoItem* pItem )
{
	if(pItem == m_pSelectedStock)
	{
		p.fillRect(rtCB,QColor(20,20,20));
	}
	int iCBCount = rtCB.width()/m_iCBHeight;
	QList<qRcvHistoryData*> list = pItem->getLastHistory(iCBCount+1);

	p.setPen(QColor(255,255,255));
	p.drawText(QRect(rtCB.left(),rtCB.top(),50,m_iCBHeight),Qt::AlignCenter,pItem->getCode());

	int iCurX = rtCB.left()+50;
	for(int i=1;i<list.size();++i)
	{
		QRect rtB = QRect(iCurX,rtCB.top(),m_iCBHeight,m_iCBHeight);
		rtB.adjust(2,2,-2,-2);
		float f = (list[i]->fClose - list[i-1]->fClose)/(list[i-1]->fClose);
		int c = f*10*255;
		if(c>0)
		{
			p.fillRect(rtB,QColor((c>255 ? 255:c),0,0));
		}
		else
		{
			p.fillRect(rtB,QColor(0,(c<-255 ? 255:(-c)),0));
		}

		iCurX = iCurX+m_iCBHeight;
	}
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

