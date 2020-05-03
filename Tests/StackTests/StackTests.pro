QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

QMAKE_CFLAGS += -std=c11

TEMPLATE = app

SOURCES += \
    tst_stacktests.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"

QMAKE_LFLAGS += "-Wl,-rpath,\'$$top_builddir/LinkedListsLib\'"
QMAKE_LFLAGS += "-Wl,-rpath,\'$$top_builddir/Utils\'"

LIBS += -L$$top_builddir/LinkedListsLib -lLinkedListsLib
LIBS += -L$$top_builddir/Utils -lUtils
