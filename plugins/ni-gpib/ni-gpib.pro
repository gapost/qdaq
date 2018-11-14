#-------------------------------------------------
#
# Project created by QtCreator 2018-04-04T15:57:10
#
#-------------------------------------------------

QT       -=  gui
QT       += script
CONFIG += plugin

INCLUDEPATH += $$PWD/../../lib/core $$PWD/../../lib/daq
DEPENDPATH += $$PWD/../../lib/core $$PWD/../../lib/daq

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../bin-release/ -llibQDaq
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../bin-debug/ -llibQDaq
else:unix: LIBS += -L$$OUT_PWD/../../lib/ -lQDaq

############## NI-488.2 ##############
NIPATH = $$PWD/../../../3rdparty/ni4882-1.6
LIBS += $$NIPATH/gpib-32.obj
INCLUDEPATH += $$NIPATH

TARGET = $$qtLibraryTarget(qdaqnigpib)
TEMPLATE = lib

win32:CONFIG(release, debug|release): DESTDIR = $$PWD/../../../bin-release/plugins
else:win32:CONFIG(debug, debug|release): DESTDIR = $$PWD/../../../bin-debug/plugins
else:unix: DESTDIR = ../../qdaq/plugins

DEFINES += NIGPIB_LIBRARY

SOURCES += nigpib.cpp

HEADERS += nigpib.h

DISTFILES += \
    qdaqnigpib.json

