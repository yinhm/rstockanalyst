/****************************************************************************
**
**
****************************************************************************/

#include <QtCore>
#include <QtGui>
#include "BaseMarketTreeModel.h"
#include "DataEngine.h"

#define	GetColorByFloat(x)	(((x)==0.0) ? QColor(191,191,191) : (((x)>0.0) ? QColor(255,80,80) : QColor(0,255,255)))

void CBaseMarketItemDelegate::paint( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
	(const_cast<QStyleOptionViewItem&>(option)).palette.setColor(QPalette::HighlightedText, index.data(Qt::ForegroundRole).value<QColor>());
//	(const_cast<QStyleOptionViewItem&>(option)).showDecorationSelected = false;
//	(const_cast<QStyleOptionViewItem&>(option)).textElideMode = Qt::ElideNone;
	//高亮显示与普通显示时的前景色一致（即选中行和为选中时候的文字颜色一样）
	return QStyledItemDelegate::paint(painter, option, index);
}


CBaseMarketTreeModel::CBaseMarketTreeModel( WORD wMarket,QObject *parent /*= 0*/ )
    : QAbstractTableModel(parent)
	, m_wMarket(wMarket)
{
    QList<QVariant> rootData;
    m_listHeader << tr("索引") << tr("代码") << tr("名称")
		<< tr("涨幅") << tr("量比") << tr("换手率（仓差）") << tr("前收") << tr("今开")
		<< tr("最高") << tr("最低") << tr("最新") << tr("总手") << tr("总额（持仓）")
		<< tr("现手") << tr("涨速") << tr("涨跌") << tr("振幅") << tr("均价") << tr("市盈率")
		<< tr("流通市值") << tr("外盘") << tr("内盘") << tr("委买量") << tr("委卖量")
		<< tr("委比") << tr("委差");
}


CBaseMarketTreeModel::~CBaseMarketTreeModel()
{

}


int CBaseMarketTreeModel::columnCount(const QModelIndex &parent) const
{
	if(parent.isValid())
		return 0;

	return m_listHeader.size();
}


QVariant CBaseMarketTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

	if(role == Qt::DisplayRole)
	{
		CStockInfoItem* itemData = m_listItems.at(index.row());

		switch(index.column())
		{
		case 0:
			{
				//索引号
				return index.row()+1;
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
				return static_cast<int>(itemData->getSellVOL());
			}
			break;
		case 21:
			{
				//内盘
				return static_cast<int>(itemData->getBuyVOL());
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
	else if(role == Qt::UserRole)
	{
		CStockInfoItem* itemData = m_listItems.at(index.row());
		return QVariant(reinterpret_cast<unsigned int>(itemData));
	}
	else if(role == Qt::TextAlignmentRole)
	{
		int iColumn = index.column();
		if(iColumn==1||iColumn==2)
			return Qt::AlignCenter;
		return static_cast<int>(Qt::AlignRight|Qt::AlignVCenter);
	}
	else if(role == Qt::TextColorRole)
	{
		CStockInfoItem* itemData = m_listItems.at(index.row());

		int iColumn = index.column();
		if(iColumn == 1)
			return QColor(127,255,191);
		else if(iColumn == 2)
			return QColor(255,255,0);
		else if(iColumn == 3 || iColumn == 15)
		{
			//涨幅
			return GetColorByFloat(itemData->getIncrease());
		}
		else if(iColumn == 7)
		{
			//今开
			float f = itemData->getOpenPrice()-itemData->getLastClose();
			return GetColorByFloat(f);
		}
		else if(iColumn == 8)
		{
			//最高
			float f = itemData->getHighPrice()-itemData->getLastClose();
			return GetColorByFloat(f);
		}
		else if(iColumn == 9)
		{
			//最低
			float f = itemData->getLowPrice()-itemData->getLastClose();
			return GetColorByFloat(f);
		}
		else if(iColumn == 10)
		{
			float f = itemData->getNewPrice()-itemData->getLastClose();
			return GetColorByFloat(f);
		}
		else if(iColumn == 17)
		{
			float f = itemData->getAvePrice()-itemData->getLastClose();
			return GetColorByFloat(f);
		}
		else if(iColumn == 20 || iColumn == 22)
			return QColor(255,80,80);
		else if(iColumn == 21 || iColumn == 23)
			return QColor(0,255,255);
		else if(iColumn == 24)
		{
			float f = itemData->getCommRatio();
			return GetColorByFloat(f);
		}
		else if(iColumn == 25)
		{
			float f = itemData->getCommSent();
			return GetColorByFloat(f);
		}

		return QColor(191,191,191);
	}

	return QVariant();
}


QVariant CBaseMarketTreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
		return m_listHeader[section];
	else if(role == Qt::TextAlignmentRole)
	{
		return Qt::AlignCenter;
	}
    return QVariant();
}

int CBaseMarketTreeModel::rowCount(const QModelIndex &parent) const
{
	if(parent.isValid())
		return 0;

    return m_listItems.size();
}

int CBaseMarketTreeModel::appendStockItem( CStockInfoItem* pItem )
{
	//是否为该Model显示的市场 或者 判断是否为重复添加
	beginInsertRows (QModelIndex(),m_listItems.size(),m_listItems.size());
	m_listItems.append(pItem);
	m_mapTable[pItem->getCode()] = m_listItems.size()-1;
	endInsertRows();

	return m_listItems.size()-1;
}

void CBaseMarketTreeModel::clearReports()
{
	beginRemoveRows(QModelIndex(),0,m_listItems.size()-1);
	m_listItems.clear();
	m_mapTable.clear();
	endRemoveRows();
}

void CBaseMarketTreeModel::updateStockItem( const QString& qsCode )
{
	if(m_wMarket == SZ_MARKET_EX && qsCode.at(0)!='3')
		return;
	if(m_wMarket == SH_MARKET_EX && qsCode.at(0)!='6')
		return;

	CStockInfoItem* pItem = CDataEngine::getDataEngine()->getStockInfoItem(qsCode);
	if((!pItem)||(pItem->getMarket()!=m_wMarket))
		return;

	int iIndex = 0;
	if(!m_mapTable.contains(pItem->getCode()))
		iIndex = appendStockItem(pItem);
	else
		iIndex = m_mapTable[qsCode];

	emit dataChanged(createIndex(iIndex,0),createIndex(iIndex,m_listHeader.count()));
}

/*
void CBaseMarketTreeModel::resetReports()
{
	QList<qRcvReportData*> listReports = CDataEngine::getDataEngine()->getBaseMarket();
	foreach(qRcvReportData* pReport,listReports)
	{
		if(pReport->wMarket==m_wMarket)
		{
			appendReport(pReport);
		}
	}

	reset();
}
*/

