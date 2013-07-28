TARGET = QRegExpTester
TEMPLATE = app
DESTDIR = "../../bin"

CONFIG += console
CONFIG += qt debug_and_release

QT += core \
    gui

HEADERS += MyWindow.h
SOURCES += main.cpp MyWindow.cpp
