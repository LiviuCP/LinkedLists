TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle

QMAKE_CFLAGS += -std=c11 -D_XOPEN_SOURCE=700

SOURCES += \
    main.c \
    ../ManualListEntry/apputils.c

HEADERS += \
    ../ManualListEntry/apputils.h

LIBS += -L$$top_builddir/LinkedListsLib -lLinkedListsLib
LIBS += -L$$top_builddir/Utils -lUtils

QMAKE_LFLAGS += "-Wl,-rpath,\'../../LinkedListsLib\'"
QMAKE_LFLAGS += "-Wl,-rpath,\'../../Utils\'"
