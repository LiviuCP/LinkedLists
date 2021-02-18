QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

QMAKE_CFLAGS += -std=c11

TEMPLATE = app

SOURCES += \
    tst_hashtabletests.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"

LIBS += -L$$top_builddir/Collections -lCollections
LIBS += -L$$top_builddir/LinkedListsLib -lLinkedListsLib
LIBS += -L$$top_builddir/Utils -lUtils

QMAKE_LFLAGS += "-Wl,-rpath,\'../../Collections\'"
QMAKE_LFLAGS += "-Wl,-rpath,\'../../LinkedListsLib\'"
QMAKE_LFLAGS += "-Wl,-rpath,\'../../Utils\'"
