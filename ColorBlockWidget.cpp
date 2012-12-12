#include "StdAfx.h"
#include "ColorBlockWidget.h"
#include "DataEngine.h"

CColorBlockWidget::CColorBlockWidget( CBaseWidget* parent /*= 0*/ )
	: CBaseWidget(parent,CBaseWidget::ColorBlock)
	, m_pMenuCustom(0)
	, m_iCBHeight(16)
	, showStockIndex(0)
{
	m_pMenuCustom = new QMenu(tr("色块图菜单"));
}

CColorBlockWidget::~CColorBlockWidget(void)
{
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
	m_listStocks.clear();
	
	disconnect(this,SLOT(updateStock(const QString&)));

	m_listStocks = CDataEngine::getDataEngine()->getStocksByBlock(block);

	foreach(CStockInfoItem* pItem,m_listStocks)
	{
		//建立更新机制
		connect(pItem,SIGNAL(stockItemHistoryChanged(const QString&)),this,SLOT(updateStock(const QString&)));
		connect(pItem,SIGNAL(stockItemMinuteChanged(const QString&)),this,SLOT(updateStock(const QString&)));
	}

	m_qsBlock = block;

	update();
	return CBaseWidget::setBlock(block);
}

void CColorBlockWidget::updateStock( const QString& code )
{
	
}

void CColorBlockWidget::paintEvent( QPaintEvent* )
{
	QPainter p(this);
	QRect rtClient = this->rect();
	drawHeader(p,QRect(rtClient.left(),rtClient.top(),rtClient.width(),20));
	drawClient(p,QRect(rtClient.left(),rtClient.top()+21,rtClient.width(),rtClient.height()-40));
	drawBottom(p,QRect(rtClient.left(),rtClient.bottom()-20,rtClient.width(),20));
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
