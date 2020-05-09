TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle

QMAKE_CFLAGS += -std=c11

SOURCES += \
    main.c \
    ../../ManualListEntry/apputils.c

HEADERS += \
    ../../ManualListEntry/apputils.h

QMAKE_LFLAGS += "-Wl,-rpath,\'$$top_builddir/LinkedListsLib\'"

LIBS += -L$$top_builddir/LinkedListsLib -lLinkedListsLib
