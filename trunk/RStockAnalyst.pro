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


HEADERS += StdAfx.h MainWindow.h \
			BaseWidget.h KLineWidget.h \
			MarketTrendWidget.h ColorBlockWidget.h BaseLiner.h \
			ColorManager.h BlockColorSettingDlg.h StockInfoWidget.h \
			SplashDlg.h CoordXBaseWidget.h BColorBlockWidget.h \
			resource.h BaseBlockWidget.h

SOURCES += main.cpp MainWindow.cpp \
			BaseWidget.cpp KLineWidget.cpp \
			MarketTrendWidget.cpp ColorBlockWidget.cpp BaseLiner.cpp \
			ColorManager.cpp BlockColorSettingDlg.cpp StockInfoWidget.cpp \
			SplashDlg.cpp CoordXBaseWidget.cpp BColorBlockWidget.cpp \
			BaseBlockWidget.cpp


RESOURCES += RStockAnalyst.qrc
RC_FILE = RStockAnalyst.rc

INCLUDEPATH += "../branches/lua-5.2.1/include/"  "../include/" "../RStockData"
LIBS += -L"../branches/lua-5.2.1/lib/"
LIBS += -L"../LIBS/"
LIBS += -llua52
LIBS += -lRStockData