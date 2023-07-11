#-------------------------------------------------
#
# Project created by QtCreator 2020-09-13T20:49:34
#
#-------------------------------------------------

QT       += core script

TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(QDaqFiltersPlugin)
# platform options
win32 {    
    # Trick Qt to not add version major to the target dll name
    TARGET_EXT = .dll
}
DESTDIR = $$OUT_PWD/../../bin/script

include(../../qdaq_version.pri)

SOURCES += filtersplugin.cpp

HEADERS += filtersplugin.h

DISTFILES += \
    filters-plugin.json

target.path = $$[QT_INSTALL_PLUGINS]/script
INSTALLS += target

LIBS += -L$$OUT_PWD/../../bin/ -lQDaqCore
INCLUDEPATH += $$PWD/../../src/core
DEPENDPATH += $$PWD/../../src/core

LIBS += -L$$OUT_PWD/../../bin/ -lQDaqFilters
INCLUDEPATH += $$PWD/../../src/filters
DEPENDPATH += $$PWD/../../src/filters


