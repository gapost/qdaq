#-------------------------------------------------
#
# Project created by QtCreator 2018-09-05T12:19:13
#
#-------------------------------------------------

QT       += core gui

TARGET = testplugin
TEMPLATE = lib
CONFIG += plugin

DESTDIR = $$[QT_INSTALL_PLUGINS]/generic

SOURCES += genericplugin.cpp

HEADERS += genericplugin.h
DISTFILES += testplugin.json

unix {
    target.path = /usr/lib
    INSTALLS += target
}
