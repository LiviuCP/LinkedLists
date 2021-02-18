QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle
CONFIG += c++14

QMAKE_CFLAGS += -std=c11

TEMPLATE = app

SOURCES += \
    tst_bitoperationstests.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"

LIBS += -L$$top_builddir/Utils -lUtils

QMAKE_LFLAGS += "-Wl,-rpath,\'../../Utils\'"
