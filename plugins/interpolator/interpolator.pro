#-------------------------------------------------
#
# Project created by QtCreator 2018-05-20T18:44:27
#
#-------------------------------------------------

QT       += script
QT       -= gui
CONFIG   += plugin

INCLUDEPATH  += ../../lib/daq ../../lib/core
LIBS += -lgsl

TARGET = $$qtLibraryTarget(qdaqinterpolator)
TEMPLATE = lib
DESTDIR = ../../qdaq/plugins

DEFINES += INTERPOLATOR_LIBRARY

SOURCES += qdaqinterpolator.cpp

HEADERS += qdaqinterpolator.h\
        interpolator_global.h

unix {
    target.path = $$[QT_INSTALL_PLUGINS]/qdaq
    INSTALLS += target
}

DISTFILES += \
    interpolator.json
