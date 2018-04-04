#-------------------------------------------------
#
# Project created by QtCreator 2018-04-04T15:57:10
#
#-------------------------------------------------

QT       -=  gui
CONFIG += plugin

INCLUDEPATH  += ../../lib/daq

############## NI-488.2 ##############
NIPATH = $$PWD/../../../3rdparty/ni4882-1.6
LIBS += $$NIPATH/gpib-32.obj
INCLUDEPATH += $$NIPATH

TARGET = $$qtLibraryTarget(qdaqnigpibplugin)
TEMPLATE = lib
DESTDIR = ../../qdaq/plugins

SOURCES += nigpib.cpp

HEADERS += nigpib.h

DISTFILES += ni-gpib.json

