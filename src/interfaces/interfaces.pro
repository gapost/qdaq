#-------------------------------------------------
#
# Project created by QtCreator 2020-09-11T16:43:00
#
#-------------------------------------------------

QT       += network script serialport
QT       -= gui

TEMPLATE = lib
DEFINES += QDAQ_LIBRARY

# platform options
TARGET = QDaqInterfaces
win32 {
    # Trick Qt to not add version major to the target dll name
    TARGET_EXT = .dll
}
DESTDIR = $$OUT_PWD/../../bin

include(../../qdaq_version.pri)

SOURCES += \
    qdaqserial.cpp \
    qdaqmodbus.cpp \
    qdaqtcpip.cpp \
    tcp_socket.cpp \
    qdaqinterfaces.cpp

HEADERS +=\
    qdaqserial.h \
    qdaqmodbus.h \
    qdaqtcpip.h \
    tcp_socket.h \
    qdaqinterfaces.h



unix {
    UBUNTU = $$system(cat /proc/version | grep -o Ubuntu)

    !contains( UBUNTU, Ubuntu) {
        SOURCES += linuxgpib.cpp
        HEADERS += linuxgpib.h
        LIBS += -lgpib
        DEFINES += LINUX_GPIB
    }
}

target.path = $$[QT_INSTALL_LIBS]
headers.files  = $${HEADERS}
headers.path   = $$[QT_INSTALL_HEADERS]/QDaq
INSTALLS += headers target

win32 {
    dlltarget.path = $$[QT_INSTALL_BINS]                                                                               
    INSTALLS += dlltarget
}

## Add module core

LIBS += -L$$OUT_PWD/../../bin/ -lQDaqCore

INCLUDEPATH += $$PWD/../core
DEPENDPATH += $$PWD/../core

######### 3rd party libs ###############

## Win32 sockets
win32 {
    LIBS += -lwsock32 -lws2_32
}

######### libmodbus ###############
LIBS += -lmodbus
## INCLUDEPATH += modbus

### NI-GPIB
# include(ni4882.pri)
