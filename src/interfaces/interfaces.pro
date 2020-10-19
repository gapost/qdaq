#-------------------------------------------------
#
# Project created by QtCreator 2020-09-11T16:43:00
#
#-------------------------------------------------

QT       += network script serialport

QT       -= gui

TARGET = QDaqInterfaces
TEMPLATE = lib

DEFINES += QDAQ_LIBRARY

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
SOURCES += linuxgpib.cpp
HEADERS += linuxgpib.h
}

win32 {
SOURCES += nigpib.cpp
HEADERS += nigpib.h
}

unix {
    target.path = $$[QT_INSTALL_LIBS]
    headers.files  = $${HEADERS}
    headers.path   = $$[QT_INSTALL_HEADERS]/QDaq
    INSTALLS += headers target
}

## Add module core

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../core/release/ -lQDaqCore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../core/debug/ -lQDaqCore
else:unix:!macx: LIBS += -L$$OUT_PWD/../core/ -lQDaqCore

INCLUDEPATH += $$PWD/../core
DEPENDPATH += $$PWD/../core

######### 3rd party libs ###############
unix {
    LIBS += -lmodbus
    INCLUDEPATH += /usr/include/modbus
    LIBS += -lgpib
}

win32 {
    ######### libmodbus ###############
    LIBS += -lwsock32 -lws2_32
    LIBS += -L$$PWD/../../../3rdparty/libmodbus-3.1.4/lib/ -llmodbus
    INCLUDEPATH += $$PWD/../../../3rdparty/libmodbus-3.1.4/src
    DEPENDPATH += $$PWD/../../../3rdparty/libmodbus-3.1.4/src
    ############## NI-488.2 ##############
    NIPATH = $$PWD/../../../3rdparty/ni4882-1.6
    LIBS += $$NIPATH/gpib-32.obj
    INCLUDEPATH += $$NIPATH
}
