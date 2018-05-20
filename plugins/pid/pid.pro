#-------------------------------------------------
#
# Project created by QtCreator 2018-05-20T18:20:48
#
#-------------------------------------------------

QT       += script
QT       -= gui
CONFIG   += plugin

INCLUDEPATH  += ../../lib/daq ../../lib/core

TARGET = $$qtLibraryTarget(qdaqpid)
TEMPLATE = lib

DEFINES += PID_LIBRARY

SOURCES += qdaqpid.cpp

HEADERS += qdaqpid.h\
        pid_global.h \
    relaytuner.h \
    isa_pid.h

unix {
    target.path = $$[QT_INSTALL_PLUGINS]/qdaq
    INSTALLS += target
}

DISTFILES += \
    pid.json
