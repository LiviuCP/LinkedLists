TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle

QMAKE_CFLAGS += -std=c11

SOURCES += \
    main.c \
    apputils.c

QMAKE_LFLAGS += "-Wl,-rpath,\'$$top_builddir/LinkedListsLib\'"
QMAKE_LFLAGS += "-Wl,-rpath,\'$$top_builddir/Utils\'"

LIBS += -L$$top_builddir/LinkedListsLib -lLinkedListsLib
LIBS += -L$$top_builddir/Utils -lUtils

HEADERS += \
    apputils.h
