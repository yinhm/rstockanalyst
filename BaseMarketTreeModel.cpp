/****************************************************************************
**
**
****************************************************************************/

#include <QtCore>
#include <QtGui>
#include "BaseMarketTreeModel.h"
#include "DataEngine.h"

CBaseMarketTreeModel::CBaseMarketTreeModel(QObject *parent)
    : QAbstractTableModel(parent)
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

    if (role != Qt::DisplayRole)
        return QVariant();

	if(index.row()>m_listItems.size())
		return QVariant();

	qRcvReportData* itemData = m_listItems.at(index.row());

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
			return itemData->qsCode;
		}
		break;
	case 2:
		{
			//股票名称
			return itemData->qsName;
		}
		break;
	case 3:
		{
			//涨幅
			if(itemData->fNewPrice<=0.0 || itemData->fLastClose<=0.0)
				return QString();
			return QString("%1%").arg((itemData->fNewPrice-itemData->fLastClose)*100.0/itemData->fLastClose,0,'f',2);
		}
		break;
	case 4:
		{
			//量比
			return QString(tr("现成交总手/〖(过去5个交易日平均每分钟成交量)×当日累计开市 时间(分)"));
		}
		break;
	case 5:
		{
			//换手率（仓差）
			return QVariant();
		}
		break;
	case 6:
		{
			//前收
			return QString("%1").arg(itemData->fLastClose,0,'f',2);
		}
		break;
	case 7:
		{
			//今开
			return QString("%1").arg(itemData->fOpen,0,'f',2);
		}
		break;
	case 8:
		{
			//最高
			return QString("%1").arg(itemData->fHigh,0,'f',2);
		}
		break;
	case 9:
		{
			//最低
			return QString("%1").arg(itemData->fLow,0,'f',2);;
		}
		break;
	case 10:
		{
			//最新
			return QString("%1").arg(itemData->fNewPrice,0,'f',2);;
		}
		break;
	case 11:
		{
			//总手
			return QString("%1").arg(itemData->fVolume,0,'f',0);
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
			return QString("%1").arg(itemData->fSellPrice[0],0,'f',2);
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

void CBaseMarketTreeModel::appendReport( qRcvReportData* data )
{
	beginInsertRows (QModelIndex(),m_listItems.size(),m_listItems.size());
	m_listItems.append(data);
	endInsertRows();
}

void CBaseMarketTreeModel::clearReports()
{
	beginRemoveRows(QModelIndex(),0,m_listItems.size()-1);
	m_listItems.clear();
	endRemoveRows();
}
