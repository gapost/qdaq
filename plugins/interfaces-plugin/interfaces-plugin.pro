#-------------------------------------------------
#
# Project created by QtCreator 2020-09-13T20:53:00
#
#-------------------------------------------------

QT       += core script serialport network

TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(QDaqInterfacesPlugin)
# platform options
win32 {   
    # Trick Qt to not add version major to the target dll name
    TARGET_EXT = .dll
}
DESTDIR = $$OUT_PWD/../../bin/script

include(../../qdaq_version.pri)

SOURCES += qdaqinterfacesplugin.cpp

HEADERS += qdaqinterfacesplugin.h
DISTFILES += interfaces-plugin.json

target.path = $$[QT_INSTALL_PLUGINS]/script
INSTALLS += target

LIBS += -L$$OUT_PWD/../../bin/ -lQDaqCore
INCLUDEPATH += $$PWD/../../src/core
DEPENDPATH += $$PWD/../../src/core


LIBS += -L$$OUT_PWD/../../bin/ -lQDaqInterfaces
INCLUDEPATH += $$PWD/../../src/interfaces
DEPENDPATH += $$PWD/../../src/interfaces
