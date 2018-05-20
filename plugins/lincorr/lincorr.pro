#-------------------------------------------------
#
# Project created by QtCreator 2018-05-20T19:07:27
#
#-------------------------------------------------

QT       += script
QT       -= gui
CONFIG   += plugin

INCLUDEPATH  += ../../lib/daq ../../lib/core

TARGET = $$qtLibraryTarget(qdaqlincorr)
TEMPLATE = lib
DESTDIR = ../../qdaq/plugins

DEFINES += LINCORR_LIBRARY

SOURCES += qdaqlinearcorrelator.cpp

HEADERS += qdaqlinearcorrelator.h\
        lincorr_global.h \
    linefit.h

unix {
    target.path = $$[QT_INSTALL_PLUGINS]/qdaq
    INSTALLS += target
}

DISTFILES += \
    lincorr.json
