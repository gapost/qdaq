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
win32 {
    TARGET = libQDaqInterfaces
    # Trick Qt to not add version major to the target dll name
    TARGET_EXT = .dll
    CONFIG(debug, debug|release) {
        DESTDIR = $$PWD/../../../bin-debug
    } else {
        DESTDIR = $$PWD/../../../bin-release
    }
}
unix {
    TARGET = QDaqInterfaces
    DESTDIR = $$OUT_PWD/../../bin
}

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
else:unix:!macx: LIBS += -L$$OUT_PWD/../../bin/ -lQDaqCore

INCLUDEPATH += $$PWD/../core
DEPENDPATH += $$PWD/../core

######### 3rd party libs ###############
unix {
    LIBS += -lmodbus
    INCLUDEPATH += /usr/include/modbus
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
