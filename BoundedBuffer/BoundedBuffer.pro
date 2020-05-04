TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG += c++11

QMAKE_CFLAGS += -std=c11

SOURCES += \
    main.cpp \
    boundedbuffer.cpp \
    buffermanipulation.cpp

QMAKE_LFLAGS += "-Wl,-rpath,\'$$top_builddir/LinkedListsLib\'"
QMAKE_LFLAGS += "-Wl,-rpath,\'$$top_builddir/Utils\'"

LIBS += -L$$top_builddir/LinkedListsLib -lLinkedListsLib
LIBS += -L$$top_builddir/Utils -lUtils

HEADERS += \
    boundedbuffer.h \
    buffermanipulation.h
