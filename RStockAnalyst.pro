TARGET = RStockAnalyst
TEMPLATE = app
DESTDIR = "../bin"

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
			MarketTrendWidget.h ColorBlockWidget.h \
			BlockColorSettingDlg.h StockInfoWidget.h \
			SplashDlg.h CoordXBaseWidget.h BColorBlockWidget.h \
			resource.h BaseBlockWidget.h KeyWizard.h \
			FuncHelper.h RadarWidget.h WatcherSettingDlg.h \
			WatcherEditDlg.h

SOURCES += main.cpp MainWindow.cpp \
			BaseWidget.cpp KLineWidget.cpp \
			MarketTrendWidget.cpp ColorBlockWidget.cpp \
			BlockColorSettingDlg.cpp StockInfoWidget.cpp \
			SplashDlg.cpp CoordXBaseWidget.cpp BColorBlockWidget.cpp \
			BaseBlockWidget.cpp KeyWizard.cpp \
			FuncHelper.cpp RadarWidget.cpp WatcherSettingDlg.cpp \
			WatcherEditDlg.cpp


RESOURCES += RStockAnalyst.qrc
RC_FILE = RStockAnalyst.rc

INCLUDEPATH += "../include/"
LIBS += -L"../bin/"
LIBS += -llua52 -lRStockData