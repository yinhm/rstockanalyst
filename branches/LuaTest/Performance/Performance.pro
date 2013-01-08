TARGET = Performance
TEMPLATE = app
CONFIG += console

QT += core

SOURCES += main.cpp

INCLUDEPATH += "../../lua-5.2.1/include/"
LIBS += -L"../../lua-5.2.1/lib/"
LIBS += -llua52