TEMPLATE = app
CONFIG += console

TARGET = RStockAnalyst
TEMPLATE = app

QT += core \
    gui \
	xml \
	sql
	

PRECOMPILED_HEADER = StdAfx.h


HEADERS += StdAfx.h StockDrv.h STKDRV.h MainWindow.h BaseMarketWidget.h \
			DataEngine.h BaseMarketTreeModel.h StockInfoItem.h \
			BaseMarketTreeView.h RMdiCtrl.h KLineWidget.h

SOURCES += main.cpp STKDRV.cpp MainWindow.cpp BaseMarketWidget.cpp \
			DataEngine.cpp BaseMarketTreeModel.cpp StockInfoItem.cpp \
			BaseMarketTreeView.cpp RMdiCtrl.cpp KLineWidget.cpp


RESOURCES += RStockAnalyst.qrc