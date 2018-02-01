#-------------------------------------------------
#
# Project created by QtCreator 2018-02-01T17:26:45
#
#-------------------------------------------------

QT       += widgets

TARGET = QDaqWidgets
TEMPLATE = lib

DEFINES += QDAQ_WIDGETS_LIBRARY

SOURCES += \
    qled.cpp

HEADERS +=\
    qled.h \
    qdaq_widgets_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
