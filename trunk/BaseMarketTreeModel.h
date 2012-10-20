/****************************************************************************
**
**
****************************************************************************/

#ifndef BASE_MARKET_TREEMODEL_H
#define BASE_MARKET_TREEMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QtCore>
#include "DataEngine.h"

class CBaseMarketTreeModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    CBaseMarketTreeModel(QObject *parent = 0);
    ~CBaseMarketTreeModel();

    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

public slots:
	void updateModelData();

private:
	QStringList m_listHeader;
	QList<qRcvReportItem*> m_listItems;
};

#endif	//BASE_MARKET_TREEMODEL_H
