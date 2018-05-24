#-------------------------------------------------
#
# Project created by QtCreator 2018-05-24T10:40:56
#
#-------------------------------------------------

QT       += script
QT       -= gui
CONFIG   += plugin


INCLUDEPATH  +=  ../../lib/core


TARGET = $$qtLibraryTarget(qdaqfilters)
TEMPLATE = lib
DESTDIR = ../../qdaq/plugins

DEFINES += FILTERS_LIBRARY

SOURCES += filterfactory.cpp \
    qdaqfopdt.cpp \
    qdaqinterpolator.cpp \
    qdaqlinearcorrelator.cpp \
    qdaqpid.cpp

HEADERS += filterfactory.h\
        filters_global.h \
    qdaqfopdt.h \
    linefit.h \
    qdaqlinearcorrelator.h \
    qdaqinterpolator.h \
    qdaqpid.h \
    relaytuner.h \
    isa_pid.h

unix {
    target.path = $$[QT_INSTALL_PLUGINS]/qdaq
    INSTALLS += target
}

DISTFILES += \
    filters.json
