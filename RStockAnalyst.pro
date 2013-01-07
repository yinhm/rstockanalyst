TARGET = RStockAnalyst
TEMPLATE = app

CONFIG += qt debug_and_release
CONFIG(debug,debug|release) {
	CONFIG += console
} else {
	DEFINES += QT_NO_DEBUG_OUTPUT
	CONFIG -= console
}

QT += core \
    gui \
	xml \
	script
	

PRECOMPILED_HEADER = StdAfx.h


HEADERS += StdAfx.h StockDrv.h STKDRV.h MainWindow.h \
			DataEngine.h StockInfoItem.h \
			BaseWidget.h KLineWidget.h \
			MarketTrendWidget.h ColorBlockWidget.h BaseLiner.h \
			ColorManager.h BlockColorSettingDlg.h StockInfoWidget.h \
			SplashDlg.h CoordXBaseWidget.h

SOURCES += main.cpp STKDRV.cpp MainWindow.cpp \
			DataEngine.cpp StockInfoItem.cpp \
			BaseWidget.cpp KLineWidget.cpp \
			MarketTrendWidget.cpp ColorBlockWidget.cpp BaseLiner.cpp \
			ColorManager.cpp BlockColorSettingDlg.cpp StockInfoWidget.cpp \
			SplashDlg.cpp CoordXBaseWidget.cpp


RESOURCES += RStockAnalyst.qrc
RC_FILE = RStockAnalyst.rc