/****************************************************************************
**
**
****************************************************************************/

#include <QtCore>
#include <QtGui>
#include "BaseMarketTreeModel.h"
#include "DataEngine.h"

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
				return itemData->getIncrease();
			}
			break;
		case 4:
			{
				return itemData->getVolumeRatio();
			}
			break;
		case 5:
			{
				return itemData->getTurnRatio();
			}
			break;
		case 6:
			{
				//前收
				return itemData->getLastClose();
			}
			break;
		case 7:
			{
				//今开
				return itemData->getOpenPrice();
			}
			break;
		case 8:
			{
				//最高
				return itemData->getHighPrice();
			}
			break;
		case 9:
			{
				//最低
				return itemData->getLowPrice();
			}
			break;
		case 10:
			{
				//最新
				return itemData->getNewPrice();
			}
			break;
		case 11:
			{
				//总手
				return itemData->getTotalVolume();
			}
			break;
		case 12:
			{
				//总额（持仓）
				return QVariant();
			}
			break;
		case 13:
			{
				//现手
//				return QString("%1").arg(itemData->fSellPrice[0],0,'f',2);
				return QString();
			}
			break;
		case 14:
			{
				//涨速
				return QVariant();
			}
			break;
		case 15:
			{
				//涨跌
				return QVariant();
			}
			break;
		case 16:
			{
				//振幅
				return QVariant();
			}
			break;
		case 17:
			{
				//均价
				return QVariant();
			}
			break;
		case 18:
			{
				//市盈率
				return QVariant();
			}
			break;
		case 19:
			{
				//流通市值
				return QVariant();
			}
			break;
		case 20:
			{
				//外盘
				return QVariant();
			}
			break;
		case 21:
			{
				//内盘
				return QVariant();
			}
			break;
		case 22:
			{
				//委买量
				return QVariant();
			}
			break;
		case 23:
			{
				//委卖量
				return QVariant();
			}
			break;
		case 24:
			{
				//委比
				return QVariant();
			}
			break;
		case 25:
			{
				//委差
				return QVariant();
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

	return QVariant();
}


QVariant CBaseMarketTreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
		return m_listHeader[section];

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