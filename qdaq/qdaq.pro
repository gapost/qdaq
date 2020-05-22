#-------------------------------------------------
#
# Project created by QtCreator 2018-01-25T17:18:35
#
#-------------------------------------------------

QT       += core gui widgets script scripttools network uitools serialport testlib

lessThan(QT_MAJOR_VERSION, 5): error("This project needs Qt5")


include(../qdaq.pri)

TARGET = qdaq
TEMPLATE = app

win32 {
CONFIG(debug, debug|release) {
    DESTDIR = $$PWD/../../bin-debug
} else {
    DESTDIR = $$PWD/../../bin-release
}
}

INCLUDEPATH += $$PWD/../lib $$PWD/../lib/core $$PWD/../lib/gui $$PWD/../lib/daq
DEPENDPATH += $$PWD/../lib $$PWD/../lib/core $$PWD/../lib/gui $$PWD/../lib/daq

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../bin-release/ -llibQDaq
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../bin-debug/ -llibQDaq
else:unix: LIBS += -L$$OUT_PWD/../lib/ -lQDaq

SOURCES += main.cpp

HEADERS  +=

unix {
    target.path = /usr/bin
    INSTALLS += target
}

DISTFILES += \
    qdaq_ide_screenshot.png


