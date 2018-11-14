#-------------------------------------------------
#
# Project created by QtCreator 2018-09-06T10:43:17
#
#-------------------------------------------------

QT       += network script serialport

QT       -= gui

CONFIG   += plugin

INCLUDEPATH += $$PWD/../../lib/core $$PWD/../../lib/daq
DEPENDPATH += $$PWD/../../lib/core $$PWD/../../lib/daq

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../bin-release/ -llibQDaq
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../bin-debug/ -llibQDaq
else:unix: LIBS += -L$$OUT_PWD/../../lib/ -lQDaq

######### libmodbus ###############
unix {
    LIBS += -lmodbus
    INCLUDEPATH += /usr/include/modbus
}

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../3rdparty/libmodbus-3.1.4/lib/ -llibmodbus
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../3rdparty/libmodbus-3.1.4/lib/ -llibmodbusd

win32 {
INCLUDEPATH += $$PWD/../../../3rdparty/libmodbus-3.1.4/src
DEPENDPATH += $$PWD/../../../3rdparty/libmodbus-3.1.4/src
CONFIG(debug, debug|release) {
    LIBS += -L$$PWD/../../../3rdparty/libmodbus-3.1.4/lib/ -llibmodbusd
} else {
    LIBS += -L$$PWD/../../../3rdparty/libmodbus-3.1.4/lib/ -llibmodbus
}
LIBS += ws2_32.lib
}

TARGET = $$qtLibraryTarget(qdaqinterfaces)
TEMPLATE = lib

win32:CONFIG(release, debug|release): DESTDIR = $$PWD/../../../bin-release/plugins
else:win32:CONFIG(debug, debug|release): DESTDIR = $$PWD/../../../bin-debug/plugins
else:unix: DESTDIR = ../../qdaq/plugins

DEFINES += INTERFACES_LIBRARY

SOURCES += interfaces.cpp \
    qdaqserial.cpp \
    qdaqmodbus.cpp \
    qdaqtcpip.cpp \
    tcp_socket.cpp

HEADERS += interfaces.h\
        interfaces_global.h \
    qdaqserial.h \
    qdaqmodbus.h \
    qdaqtcpip.h \
    tcp_socket.h

unix {
    target.path = $$[QT_INSTALL_PLUGINS]/qdaq
    INSTALLS += target
}

DISTFILES += \
    qdaqinterfaces.json

