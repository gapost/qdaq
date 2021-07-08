#-------------------------------------------------
#
# Project created by QtCreator 2018-01-25T17:18:35
#
#-------------------------------------------------

QT       += gui widgets script

TARGET = qdaq
TEMPLATE = app

# platform options
win32 {
    CONFIG(debug, debug|release) {
        DESTDIR = $$PWD/../../../bin-debug
    } else {
        DESTDIR = $$PWD/../../../bin-release
    }
}
unix {
    DESTDIR = $$OUT_PWD/../../bin
}

SOURCES += main.cpp

HEADERS  +=

unix {
    target.path = /usr/bin
    INSTALLS += target
}

RESOURCES += \
    qdaq.qrc



win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../core/release/ -lQDaqCore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../core/debug/ -lQDaqCore
else:unix:!macx: LIBS += -L$$OUT_PWD/../../bin/ -lQDaqCore

INCLUDEPATH += $$PWD/../../src/core
DEPENDPATH += $$PWD/../../src/core

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../gui/release/ -lQDaqGui
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../gui/debug/ -lQDaqGui
else:unix:!macx: LIBS += -L$$OUT_PWD/../../bin/ -lQDaqGui

INCLUDEPATH += $$PWD/../../src/gui $$PWD/../../src/gui/qconsolewidget/src
DEPENDPATH += $$PWD/../../src/gui
