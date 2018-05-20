#-------------------------------------------------
#
# Project created by QtCreator 2018-05-20T20:19:33
#
#-------------------------------------------------

QT       += script
QT       -= gui
CONFIG   += plugin

INCLUDEPATH  += ../../lib/daq ../../lib/core

TARGET = $$qtLibraryTarget(qdaqfopdt)
TEMPLATE = lib
DESTDIR = ../../qdaq/plugins

DEFINES += FOPDT_LIBRARY

SOURCES += qdaqfopdt.cpp

HEADERS += qdaqfopdt.h\
        fopdt_global.h

unix {
    target.path = $$[QT_INSTALL_PLUGINS]/qdaq
    INSTALLS += target
}

DISTFILES += \
    fopdt.json
