#-------------------------------------------------
#
# Project created by QtCreator 2018-01-25T17:18:35
#
#-------------------------------------------------

#QT       += core gui widgets script scripttools network uitools serialport testlib
QT       += gui widgets script

lessThan(QT_MAJOR_VERSION, 5): error("This project needs Qt5")

TARGET = qdaq
TEMPLATE = app

win32 {
CONFIG(debug, debug|release) {
    DESTDIR = $$PWD/../../bin-debug
} else {
    DESTDIR = $$PWD/../../bin-release
}
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
else:unix:!macx: LIBS += -L$$OUT_PWD/../core/ -lQDaqCore

INCLUDEPATH += $$PWD/../core
DEPENDPATH += $$PWD/../core

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../gui/release/ -lQDaqGui
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../gui/debug/ -lQDaqGui
else:unix:!macx: LIBS += -L$$OUT_PWD/../gui/ -lQDaqGui

INCLUDEPATH += $$PWD/../gui
DEPENDPATH += $$PWD/../gui
