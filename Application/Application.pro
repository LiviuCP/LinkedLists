TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle

SOURCES += \
    main.c

QMAKE_LFLAGS += "-Wl,-rpath,\'$$top_builddir/LinkedListsLib\'"

LIBS += -L$$top_builddir/LinkedListsLib -lLinkedListsLib
