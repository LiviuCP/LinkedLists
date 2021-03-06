TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle

QMAKE_CFLAGS += -std=c11

SOURCES += \
    main.c \
    apputils.c

LIBS += -L$$top_builddir/LinkedListsLib -lLinkedListsLib
LIBS += -L$$top_builddir/Utils -lUtils

HEADERS += \
    apputils.h

QMAKE_LFLAGS += -Wl,-rpath,"\'../../LinkedListsLib\'"
QMAKE_LFLAGS += -Wl,-rpath,"\'../../Utils\'"
