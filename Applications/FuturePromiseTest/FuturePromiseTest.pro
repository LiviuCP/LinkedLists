TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG += c++11

QMAKE_CFLAGS += -std=c11

SOURCES += \
    main.cpp \
    ../ASyncTest/asyncutils.cpp \
    ../ManualListEntry/apputils.c

LIBS += -L$$top_builddir/LinkedListsLib -lLinkedListsLib
LIBS += -L$$top_builddir/Utils -lUtils

HEADERS += \
    ../ASyncTest/asyncutils.h \
    ../ManualListEntry/apputils.h

QMAKE_LFLAGS += "-Wl,-rpath,\'../../LinkedListsLib\'"
QMAKE_LFLAGS += "-Wl,-rpath,\'../../Utils\'"
