#-------------------------------------------------
#
# Project created by QtCreator 2020-09-13T20:53:00
#
#-------------------------------------------------

QT       += core script serialport network

TARGET = QDaqInterfacesPlugin
TEMPLATE = lib
CONFIG += plugin

DESTDIR = ../../qdaq/script

SOURCES += qdaqinterfacesplugin.cpp

HEADERS += qdaqinterfacesplugin.h
DISTFILES += interfaces-plugin.json

unix {
    target.path = $$[QT_INSTALL_PLUGINS]/script
    INSTALLS += target
}

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../core/release/ -lQDaqCore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../core/debug/ -lQDaqCore
else:unix:!macx: LIBS += -L$$OUT_PWD/../../core/ -lQDaqCore

INCLUDEPATH += $$PWD/../../core
DEPENDPATH += $$PWD/../../core


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../interfaces/release/ -lQDaqInterfaces
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../interfaces/debug/ -lQDaqInterfaces
else:unix:!macx: LIBS += -L$$OUT_PWD/../../interfaces/ -lQDaqInterfaces

INCLUDEPATH += $$PWD/../../interfaces
DEPENDPATH += $$PWD/../../interfaces
