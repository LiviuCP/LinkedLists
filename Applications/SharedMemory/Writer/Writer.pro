TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle

QMAKE_CFLAGS += -std=c11

LIBS += -lrt -lpthread

SOURCES += \
    main.c
