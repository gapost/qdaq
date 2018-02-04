#-------------------------------------------------
#
# Project created by QtCreator 2018-01-25T17:18:35
#
#-------------------------------------------------

QT       += core gui widgets script scripttools network uitools serialport

lessThan(QT_MAJOR_VERSION, 5): error("This project needs Qt5")


include(../qdaq.pri)

TARGET = qdaq
TEMPLATE = app


SOURCES += main.cpp

HEADERS  +=


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../lib/release/ -lQDaq
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../lib/debug/ -lQDaq
else:unix: LIBS += -L$$OUT_PWD/../lib/ -lQDaq

INCLUDEPATH += $$PWD/../lib
DEPENDPATH += $$PWD/../lib



win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../widgets/src/release/ -lQDaqWidgets
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../widgets/src/debug/ -lQDaqWidgets
else:unix: LIBS += -L$$OUT_PWD/../widgets/src/ -lQDaqWidgets

INCLUDEPATH += $$PWD/../widgets/src
DEPENDPATH += $$PWD/../widgets/src
