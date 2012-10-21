#pragma once
#include <QtGui>
#include "BaseMarketTreeModel.h"

class CBaseMarketWidget : public QWidget
{
	Q_OBJECT
public:
	CBaseMarketWidget();
	~CBaseMarketWidget(void);


public slots:
	void updateBaseMarket();
	void treeItemClicked(const QModelIndex& index);

private:
	//上证A股
	CBaseMarketTreeModel* m_pModelSHA;
	QTreeView* m_pViewSHA;

	//深圳指数
	CBaseMarketTreeModel* m_pModelSZ;
	QTreeView* m_pViewSZ;

	//香港股市
	CBaseMarketTreeModel* m_pModelHK;
	QTreeView* m_pViewHK;

	//其它
	CBaseMarketTreeModel* m_pModelOther;
	QTreeView* m_pViewOther;
};

