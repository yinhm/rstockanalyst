#include "StdAfx.h"
#include "BColorBlockWidget.h"


CBColorBlockWidget::CBColorBlockWidget(CBaseWidget* parent /*= 0*/ )
	: CBaseBlockWidget(parent,WidgetBColorBlock)
	, m_iTitleHeight(16)
	, m_iBottomHeight(16)
	, showStockIndex(0)
	, m_pSelectedStock(0)
{
	connect(&m_timerUpdateUI,SIGNAL(timeout()),this,SLOT(updateColorBlockData()));
	m_timerUpdateUI.start(1000);
}


CBColorBlockWidget::~CBColorBlockWidget(void)
{
	clearTmpData();
}

bool CBColorBlockWidget::loadPanelInfo( const QDomElement& eleWidget )
{
	if(!CBaseBlockWidget::loadPanelInfo(eleWidget))
		return false;

	updateData();
	return true;
}

bool CBColorBlockWidget::savePanelInfo( QDomDocument& doc,QDomElement& eleWidget )
{
	if(!CBaseBlockWidget::savePanelInfo(doc,eleWidget))
		return false;

	return true;
}

void CBColorBlockWidget::updateData()
{
	updateSortMode(false);
	if(m_typeCircle<Day)
	{
		//分笔数据10秒中更新一次
		m_timerUpdateUI.stop();
		m_timerUpdateUI.start(1000*10);
	}
	else
	{
		//日线数据30秒中更新一次
		m_timerUpdateUI.stop();
		m_timerUpdateUI.start(1000*30);
	}
	return/* CBaseBlockWidget::updateData()*/;
}

void CBColorBlockWidget::clearTmpData()
{

}

void CBColorBlockWidget::updateSortMode( bool bSelFirst )
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

void CBColorBlockWidget::getKeyWizData( const QString& keyword,QList<KeyWizData*>& listRet )
{

}

void CBColorBlockWidget::keyWizEntered( KeyWizData* pData )
{

}

void CBColorBlockWidget::setBlock( const QString& block )
{

}

void CBColorBlockWidget::updateColorBlockData()
{

}

void CBColorBlockWidget::updateShowMap()
{

}

void CBColorBlockWidget::clickedBlock( CBlockInfoItem* pItem )
{

}

void CBColorBlockWidget::paintEvent( QPaintEvent* e )
{

}

void CBColorBlockWidget::mouseMoveEvent( QMouseEvent* e )
{

}

void CBColorBlockWidget::mousePressEvent( QMouseEvent* e )
{

}

void CBColorBlockWidget::wheelEvent( QWheelEvent* e )
{

}

void CBColorBlockWidget::keyPressEvent( QKeyEvent* e )
{

}

void CBColorBlockWidget::drawColocBlock( QPainter& p,int iY, QVector<float>& vColor,QVector<float>& vHeight,QVector<float>& vWidth )
{

}

QMenu* CBColorBlockWidget::getCustomMenu()
{
	CBaseBlockWidget::getCustomMenu();

	return m_pMenuCustom;
}

void CBColorBlockWidget::drawHeader( QPainter& p,const QRect& rtHeader )
{

}

void CBColorBlockWidget::drawClient( QPainter& p,const QRect& rtClient )
{

}

void CBColorBlockWidget::drawBottom( QPainter& p,const QRect& rtBottom )
{

}

void CBColorBlockWidget::drawBlock( QPainter& p,const QRect& rtCB,CBlockInfoItem* pItem )
{

}

QRect CBColorBlockWidget::rectOfBlock( CBlockInfoItem* pItem )
{
	return QRect();
}

CBlockInfoItem* CBColorBlockWidget::hitTestStock( const QPoint& ptPoint ) const
{

	return NULL;
}

RStockData* CBColorBlockWidget::hitTestCBItem( const QPoint& ptPoint ) const
{
	return NULL;
}
