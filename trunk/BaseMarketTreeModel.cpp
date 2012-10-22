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
				/*量比计算：
					量比＝现成交总手/（过去5日平均每分钟成交量*当日累计开市时间（分）） 
					当量比大于1时，说明当日每分钟的平均成交量要大于过去5日的平均数值，交易比过去5日火爆；
					而当量比小于1时，说明现在的成交比不上过去5日的平均水平。
				*/
				if(itemData->mapHistorys.size()<5)
					return QVariant();

				//判断最新的数据是否是今天开市后的数据
				time_t tmSeconds = CDataEngine::getOpenSeconds(itemData->tmTime);
				if(tmSeconds<1)
					return QVariant();

				time_t* pLast5Day = CDataEngine::getLast5DayTime();
				float fVolume5 = 0.0;
				for(int i=0;i<5;++i)
				{
					if(!itemData->mapHistorys.contains(pLast5Day[i]))
						return QVariant();
					fVolume5 = (fVolume5 + itemData->mapHistorys.value(pLast5Day[i]).fVolume);
				}

				return (itemData->fVolume)/((fVolume5/((CDataEngine::getOpenSeconds()/60)*5))*(tmSeconds/60));
			}
			break;
		case 5:
			{
				/*换手率（仓差）
					换手率=某一段时期内的成交量/发行总股数*100%
					（在中国：成交量/流通总股数*100%）
				*/
				return QVariant()/*itemData->fVolume/*/;
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
	}
	else if(role == Qt::UserRole)
	{
		qRcvReportData* itemData = m_listItems.at(index.row());
		return QVariant::fromValue(itemData);
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

bool CBaseMarketTreeModel::appendReport( qRcvReportData* data )
{
	//是否为该Model显示的市场 或者 判断是否为重复添加
	if((data->wMarket != m_wMarket)||(m_mapTable.contains(data->qsCode)))
		return false;

	beginInsertRows (QModelIndex(),m_listItems.size(),m_listItems.size());
	m_listItems.append(data);
	m_mapTable[data->qsCode] = m_listItems.size()-1;
	endInsertRows();

	return true;
}

void CBaseMarketTreeModel::clearReports()
{
	beginRemoveRows(QModelIndex(),0,m_listItems.size()-1);
	m_listItems.clear();
	m_mapTable.clear();
	endRemoveRows();
}

void CBaseMarketTreeModel::updateBaseMarket( const QString& qsCode )
{
	qRcvReportData* pReport = CDataEngine::getDataEngine()->getBaseMarket(qsCode);
	if((!pReport)||(pReport->wMarket!=m_wMarket))
		return;

	appendReport(pReport);

	int iIndex = m_mapTable[qsCode];
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