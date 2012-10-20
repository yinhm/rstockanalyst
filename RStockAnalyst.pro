TEMPLATE = app
CONFIG += console

TARGET = RStockAnalyst
TEMPLATE = app

QT += core \
    gui \
	xml
	

PRECOMPILED_HEADER = StdAfx.h


HEADERS += StdAfx.h StockDrv.h STKDRV.h MainWindow.h BaseMarketWidget.h

SOURCES += main.cpp STKDRV.cpp MainWindow.cpp BaseMarketWidget.cpp