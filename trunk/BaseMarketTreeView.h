#ifndef BASE_MARKET_TREEVIEW_H
#define BASE_MARKET_TREEVIEW_H
#include "BaseMarketTreeModel.h"
#include <QTreeView>
#include <QKeyEvent>


class CBaseMarketTreeView : public QTreeView
{
	Q_OBJECT
public:
	CBaseMarketTreeView(WORD wMarket,QWidget* parent = 0);
	~CBaseMarketTreeView(void);

protected:
	virtual void keyPressEvent(QKeyEvent* event);

public:
	CBaseMarketTreeModel* getModel(){return m_pModel;}

protected slots:
	void horizonScolled(int v);

private:
	CBaseMarketTreeModel* m_pModel;
	QStack<int> stackHiddenColumn;
	int iHBarValue;
};


#endif	//BASE_MARKET_TREEVIEW_H