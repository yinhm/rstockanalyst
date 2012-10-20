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
	void updateBaseMarket()
	{
		m_pTreemModel->updateModelData();
	}
private:
	CBaseMarketTreeModel* m_pTreemModel;
	QTreeView* m_pTreeView;
};

