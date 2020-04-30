QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle
CONFIG += c++14

QMAKE_CFLAGS += -std=c11

TEMPLATE = app

SOURCES += \
    tst_othertests.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"

QMAKE_LFLAGS += "-Wl,-rpath,\'$$top_builddir/Utils\'"

LIBS += -L$$top_builddir/Utils -lUtils
