#-------------------------------------------------
#
# Project created by QtCreator 2018-09-06T10:43:17
#
#-------------------------------------------------

QT       += network script serialport

QT       -= gui

CONFIG   += plugin

INCLUDEPATH  +=  ../../lib/core ../../lib/daq

unix {
    LIBS += -lmodbus
    INCLUDEPATH += /usr/include/modbus
}

win32 {
############# libmodbus ##############
LIBMODBUS_PATH = $$PWD/../3rdparty/libmodbus-3.1.4-msvc2015
INCLUDEPATH += $$LIBMODBUS_PATH/include
LIBS += -L$$LIBMODBUS_PATH/lib -lmodbus
}

TARGET = $$qtLibraryTarget(qdaqinterfaces)
TEMPLATE = lib
DESTDIR = ../../qdaq/plugins

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
