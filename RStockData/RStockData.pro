TARGET = RStockData
TEMPLATE = lib
DEFINES += QWT_DLL

CONFIG += qt debug_and_release


QT += core xml

PRECOMPILED_HEADER = StdAfx.h


HEADERS += StdAfx.h StockDrv.h STKDRV.h\
			DataEngine.h StockInfoItem.h \
			BlockInfoItem.h

SOURCES += STKDRV.cpp \
			DataEngine.cpp StockInfoItem.cpp \
			BlockInfoItem.cpp

INCLUDEPATH += "../branches/lua-5.2.1/include/"
LIBS += -L"../branches/lua-5.2.1/lib/"
LIBS += -llua52