TARGET = RStockData
TEMPLATE = lib
DESTDIR = "../bin"
DEFINES += RSD_LIBRARY

CONFIG += qt debug_and_release


QT += core xml

PRECOMPILED_HEADER = StdAfx.h


HEADERS += StdAfx.h ../include/StockDrv.h ../include/STKDRV.h\
			../include/DataEngine.h ../include/StockInfoItem.h \
			../include/BlockInfoItem.h ../include/rsd_global.h

SOURCES += STKDRV.cpp \
			DataEngine.cpp StockInfoItem.cpp \
			BlockInfoItem.cpp

INCLUDEPATH += "../branches/lua-5.2.1/include/" "../include/"
LIBS += -L"../branches/lua-5.2.1/lib/"
LIBS += -llua52