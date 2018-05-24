#-------------------------------------------------
#
# Project created by QtCreator 2018-05-24T10:40:56
#
#-------------------------------------------------

QT       += script

QT       -= gui

TARGET = filters
TEMPLATE = lib

DEFINES += FILTERS_LIBRARY

SOURCES += filterfactory.cpp

HEADERS += filterfactory.h\
        filters_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
