#-------------------------------------------------
#
# Project created by QtCreator 2018-01-25T17:18:35
#
#-------------------------------------------------

QT       += core gui widgets script scripttools network uitools serialport testlib

lessThan(QT_MAJOR_VERSION, 5): error("This project needs Qt5")


include(../qdaq.pri)
include(../gitversion.pri)

TARGET = qdaq
TEMPLATE = app


SOURCES += main.cpp

HEADERS  +=

unix {
    target.path = /usr/bin
    INSTALLS += target
}


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../lib/release/ -lQDaq
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../lib/debug/ -lQDaq
else:unix: LIBS += -L$$OUT_PWD/../lib/ -lQDaq

INCLUDEPATH += $$PWD/../lib $$PWD/../lib/core $$PWD/../lib/gui $$PWD/../lib/daq
DEPENDPATH += $$PWD/../lib $$PWD/../lib/core $$PWD/../lib/gui $$PWD/../lib/daq

