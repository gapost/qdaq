#-------------------------------------------------
#
# Project created by QtCreator 2020-09-13T20:49:34
#
#-------------------------------------------------

QT       += core script

TEMPLATE = lib
CONFIG += plugin

# platform options
win32 {
    TARGET = $$qtLibraryTarget(QDaqFiltersPlugin)
    # Trick Qt to not add version major to the target dll name
    TARGET_EXT = .dll
    CONFIG(debug, debug|release) {
        DESTDIR = $$PWD/../../../bin-debug
    } else {
        DESTDIR = $$PWD/../../../bin-release
    }
}
unix {
    TARGET = $$qtLibraryTarget(QDaqFiltersPlugin)
    DESTDIR = $$OUT_PWD/../../bin/script
}

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
else:unix:!macx: LIBS += -L$$OUT_PWD/../../bin/ -lQDaqCore

INCLUDEPATH += $$PWD/../../src/core
DEPENDPATH += $$PWD/../../src/core


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../filters/release/ -lQDaqFilters
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../filters/debug/ -lQDaqFilters
else:unix:!macx: LIBS += -L$$OUT_PWD/../../bin/ -lQDaqFilters

INCLUDEPATH += $$PWD/../../src/filters
DEPENDPATH += $$PWD/../../src/filters


