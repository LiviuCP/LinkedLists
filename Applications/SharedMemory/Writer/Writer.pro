TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle

QMAKE_CFLAGS += -std=c11

unix:!macx {
    LIBS += -lrt
}

SOURCES += \
    main.c
