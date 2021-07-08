#-------------------------------------------------
#
# Project created by QtCreator 2020-09-13T20:53:00
#
#-------------------------------------------------

QT       += core script serialport network

TEMPLATE = lib
CONFIG += plugin

# platform options
win32 {
    TARGET = $$qtLibraryTarget(QDaqInterfacesPlugin)
    # Trick Qt to not add version major to the target dll name
    TARGET_EXT = .dll
    CONFIG(debug, debug|release) {
        DESTDIR = $$PWD/../../../bin-debug
    } else {
        DESTDIR = $$PWD/../../../bin-release
    }
}
unix {
    TARGET = $$qtLibraryTarget(QDaqInterfacesPlugin)
    DESTDIR = $$OUT_PWD/../../bin/script
}

SOURCES += qdaqinterfacesplugin.cpp

HEADERS += qdaqinterfacesplugin.h
DISTFILES += interfaces-plugin.json

unix {
    target.path = $$[QT_INSTALL_PLUGINS]/script
    INSTALLS += target
}

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../core/release/ -lQDaqCore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../core/debug/ -lQDaqCore
else:unix:!macx: LIBS += -L$$OUT_PWD/../../bin/ -lQDaqCore

INCLUDEPATH += $$PWD/../../src/core
DEPENDPATH += $$PWD/../../src/core


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../interfaces/release/ -lQDaqInterfaces
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../interfaces/debug/ -lQDaqInterfaces
else:unix:!macx: LIBS += -L$$OUT_PWD/../../bin/ -lQDaqInterfaces

INCLUDEPATH += $$PWD/../../src/interfaces
DEPENDPATH += $$PWD/../../src/interfaces
