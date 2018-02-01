#-------------------------------------------------
#
# Project created by QtCreator 2018-01-25T17:18:35
#
#-------------------------------------------------

QT       += core gui widgets script scripttools network uitools serialport

lessThan(QT_MAJOR_VERSION, 5): error("This app needs Qt5")

include(../qdaq.pri)

TARGET = qdaq
TEMPLATE = app


SOURCES += main.cpp

HEADERS  +=

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../lib/release/ -lqdaq
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../lib/debug/ -lqdaq
else:unix: LIBS += -L$$OUT_PWD/../lib/ -lqdaq

INCLUDEPATH += $$PWD/../lib $$PWD/../lib/core $$PWD/../lib/gui $$PWD/../lib/daq
DEPENDPATH += $$PWD/../lib
