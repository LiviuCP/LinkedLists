#-------------------------------------------------
#
# Project created by QtCreator 2020-04-09T12:56:35
#
#-------------------------------------------------

QT       -= gui

QMAKE_CFLAGS += -std=c11

TARGET = LinkedListsLib
TEMPLATE = lib

DEFINES += LINKEDLISTSLIB_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        linkedlistslib.cpp \
        linkedlist.c \
        sort.c

HEADERS += \
        linkedlistslib.h \
        linkedlistslib_global.h \
        linkedlist.h \
        listsortutils.h \
        sort.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

QMAKE_LFLAGS += "-Wl,-rpath,\'$$top_builddir/Utils\'"

LIBS += -L$$top_builddir/Utils -lUtils
