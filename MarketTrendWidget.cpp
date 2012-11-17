#include "StdAfx.h"
#include "MarketTrendWidget.h"

CMarketTrendWidget::CMarketTrendWidget( CBaseWidget* parent /*= 0*/ )
	: CBaseWidget(parent,CBaseWidget::MarketTrend)
	, m_iHeaderHeight(16)
	, m_iItemHeight(16)
	, m_iBottomHeight(20)
	, m_rtHeader(0,0,0,0)
	, m_rtClient(0,0,0,0)
	, m_rtBottom(0,0,0,0)
{
	m_listHeader << tr("索引") << tr("代码") << tr("名称")
		<< tr("涨幅") << tr("量比") << tr("换手率（仓差）") << tr("前收") << tr("今开")
		<< tr("最高") << tr("最低") << tr("最新") << tr("总手") << tr("总额（持仓）")
		<< tr("现手") << tr("涨速") << tr("涨跌") << tr("振幅") << tr("均价") << tr("市盈率")
		<< tr("流通市值") << tr("外盘") << tr("内盘") << tr("委买量") << tr("委卖量")
		<< tr("委比") << tr("委差");

	m_pMenuCustom = new QMenu(tr("市场行情图菜单"));
}

CMarketTrendWidget::~CMarketTrendWidget(void)
{
}

void CMarketTrendWidget::paintEvent( QPaintEvent* e )
{
	QPainter p(this);
	drawHeaders(p);
	p.fillRect(m_rtClient,QColor(255,255,0));
	p.fillRect(m_rtBottom,QColor(255,255,255));
}

void CMarketTrendWidget::resizeEvent( QResizeEvent* e)
{
	updateDrawRect();
	return CBaseWidget::resizeEvent(e);
}

QMenu* CMarketTrendWidget::getCustomMenu()
{
	return m_pMenuCustom;
}

void CMarketTrendWidget::updateDrawRect()
{
	QRect rtClient = this->rect();
	m_rtHeader = QRect(rtClient.topLeft(),QSize(rtClient.width(),m_iHeaderHeight));
	m_rtBottom = QRect(rtClient.left(),rtClient.bottom()-m_iBottomHeight,rtClient.width(),m_iBottomHeight);
	m_rtClient = QRect(m_rtHeader.bottomLeft(),QSize(rtClient.width(),rtClient.height()-m_iBottomHeight-m_iHeaderHeight));
}

void CMarketTrendWidget::drawHeaders( QPainter& p )
{
	p.fillRect(m_rtHeader,QColor(255,0,0));
}
