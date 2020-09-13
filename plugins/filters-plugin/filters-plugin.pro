#-------------------------------------------------
#
# Project created by QtCreator 2020-09-13T20:49:34
#
#-------------------------------------------------

QT       += core script

TARGET = QDaqFiltersPlugin
TEMPLATE = lib
CONFIG += plugin

DESTDIR = ../../qdaq/script

SOURCES += filtersplugin.cpp

HEADERS += filtersplugin.h

DISTFILES += \
    filters-plugin.json

unix {
    target.path = $$[QT_INSTALL_PLUGINS]/script
    INSTALLS += target
}

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../core/release/ -lQDaqCore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../core/debug/ -lQDaqCore
else:unix:!macx: LIBS += -L$$OUT_PWD/../../core/ -lQDaqCore

INCLUDEPATH += $$PWD/../../core
DEPENDPATH += $$PWD/../../core


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../filters/release/ -lQDaqFilters
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../filters/debug/ -lQDaqFilters
else:unix:!macx: LIBS += -L$$OUT_PWD/../../filters/ -lQDaqFilters

INCLUDEPATH += $$PWD/../../filters
DEPENDPATH += $$PWD/../../filters


