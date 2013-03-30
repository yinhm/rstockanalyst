TARGET = RStockData
TEMPLATE = lib
DESTDIR = "../bin"
DEFINES += RSD_LIBRARY

CONFIG += qt debug_and_release


QT += core xml

PRECOMPILED_HEADER = StdAfx.h


HEADERS += StdAfx.h ../include/StockDrv.h ../include/STKDRV.h\
			../include/DataEngine.h ../include/StockInfoItem.h \
			../include/BlockInfoItem.h ../include/rsd_global.h \
			../include/ColorManager.h \
			../include/RLuaEx.h ../include/RStockFunc.h Hz2Py.h

SOURCES += STKDRV.cpp \
			DataEngine.cpp StockInfoItem.cpp ColorManager.cpp \
			BlockInfoItem.cpp RLuaEx.cpp Hz2Py.cpp

INCLUDEPATH += "../include/"
LIBS += -L"../bin/"
LIBS += -llua52