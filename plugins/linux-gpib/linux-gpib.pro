#-------------------------------------------------
#
# Project created by QtCreator 2018-02-21T17:44:36
#
#-------------------------------------------------

QT       -= gui
QT       += script network serialport
CONFIG   += plugin
INCLUDEPATH  += ../../lib/daq ../../lib/core
LIBS += -lgpib
TARGET = $$qtLibraryTarget(qdaqlinuxgpib)
TEMPLATE = lib
DESTDIR = ../../qdaq/plugins

DEFINES += LINUXGPIB_LIBRARY

SOURCES += linuxgpib.cpp

HEADERS += linuxgpib.h\
        linux-gpib_global.h

unix {
    target.path = $$[QT_INSTALL_PLUGINS]/qdaq
    headers.files  = $${HEADERS}
    headers.path   = $$[QT_INSTALL_HEADERS]/QDaq
    INSTALLS += headers target
}

DISTFILES += \
    qdaqlinuxgpib.json


