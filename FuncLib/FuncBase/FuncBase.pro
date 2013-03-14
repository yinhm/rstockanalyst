TARGET = FuncBase
TEMPLATE = lib
DESTDIR = "../../bin/plugin"

CONFIG += qt debug_and_release

QT += core \
    gui 
	

PRECOMPILED_HEADER = ../include/StdAfx.h


HEADERS += ../include/StdAfx.h FuncBase.h

SOURCES += FuncBase.cpp


INCLUDEPATH += "../../branches/lua-5.2.1/include/" "../include/" "../../RStockData"
LIBS += -L"../../branches/lua-5.2.1/lib/"
LIBS += -L"../../bin/"
LIBS += -llua52
LIBS += -lRStockData