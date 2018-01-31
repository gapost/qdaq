#-------------------------------------------------
#
# Project created by QtCreator 2018-01-09T23:05:54
#
#-------------------------------------------------

QT       += core gui widgets script scripttools network uitools serialport

lessThan(QT_MAJOR_VERSION, 5): error("This app needs Qt5")

include(../qdaq.pri)

## Qt-solutions
include(3rdparty/qt-solutions/qtpropertybrowser/src/qtpropertybrowser.pri)

TARGET = qdaq
TEMPLATE = lib

SOURCES += \
    core/QDaqSession.cpp \
    core/QDaqObject.cpp \
    core/QDaqRoot.cpp \
    core/QDaqH5Serialize.cpp \
    core/QDaqTypes.cpp \
    core/QDaqJob.cpp \
    gui/QDaqConsole.cpp \
    gui/QConsoleWidget.cpp \
    core/QDaqLogFile.cpp \
    core/QDaqChannel.cpp \
    core/QDaqDataBuffer.cpp \
    core/QDaqBufferProto.cpp \
    gui/QDaqObjectController.cpp \
    gui/QDaqObjectBrowser.cpp \
    gui/QDaqErrorLog.cpp \
    gui/QDaqIde.cpp \
    gui/QDaqScriptEditor.cpp \
    gui/QDaqDelegates.cpp \
    gui/QDaqWindow.cpp \
    daq/tcp_socket.cpp \
    daq/QDaqInterface.cpp \
    daq/QDaqDevice.cpp

HEADERS  += \
    core/QDaqSession.h \
    core/QDaqObject.h \
    core/QDaqGlobal.h \
    core/QDaqRoot.h \
    core/QDaqTypes.h \
    core/os_utils.h \
    core/os_utils_linux.h \
    core/os_utils_win32.h \
    core/QDaqJob.h \
    core/math_util.h \
    gui/QConsoleWidget.h \
    gui/QDaqConsole.h \
    core/QDaqLogFile.h \
    core/QDaqChannel.h \
    core/QDaqEnumHelper.h \
    core/QDaqDataBuffer.h \
    core/QDaqBufferProto.h \
    gui/QDaqObjectBrowser.h \
    gui/QDaqObjectController.h \
    gui/QDaqErrorLog.h \
    gui/QDaqIde.h \
    gui/QDaqScriptEditor.h \
    gui/QDaqDelegates.h \
    gui/QDaqWindow.h \
    daq/tcp_socket.h \
    daq/QDaqInterface.h \
    daq/QDaqDevice.h

## JSedit
SOURCES += 3rdparty/jsedit/jsedit.cpp
HEADERS += 3rdparty/jsedit/jsedit.h
INCLUDEPATH += 3rdparty/jsedit

INCLUDEPATH += ./core ./gui ./daq

DISTFILES += \
    qdaq.pri \
    .gitignore

RESOURCES += \
    qdaq.qrc
