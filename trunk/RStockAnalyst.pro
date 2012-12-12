TEMPLATE = app
CONFIG += console

TARGET = RStockAnalyst
TEMPLATE = app

QT += core \
    gui \
	xml \
	script
	

PRECOMPILED_HEADER = StdAfx.h


HEADERS += StdAfx.h StockDrv.h STKDRV.h MainWindow.h \
			DataEngine.h StockInfoItem.h \
			BaseWidget.h KLineWidget.h \
			MarketTrendWidget.h ColorBlockWidget.h BaseLiner.h \
			ColorManager.h

SOURCES += main.cpp STKDRV.cpp MainWindow.cpp \
			DataEngine.cpp StockInfoItem.cpp \
			BaseWidget.cpp KLineWidget.cpp \
			MarketTrendWidget.cpp ColorBlockWidget.cpp BaseLiner.cpp \
			ColorManager.cpp


RESOURCES += RStockAnalyst.qrc