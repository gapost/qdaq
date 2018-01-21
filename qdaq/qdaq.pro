#-------------------------------------------------
#
# Project created by QtCreator 2018-01-09T23:05:54
#
#-------------------------------------------------

QT       += core gui widgets script scripttools network uitools

lessThan(QT_MAJOR_VERSION, 5): error("This app needs Qt5")

include(qdaq.pri)

TARGET = qdaq
TEMPLATE = app

SOURCES += main.cpp\
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
    gui/jsedit.cpp \
    gui/QDaqObjectController.cpp \
    gui/QDaqObjectBrowser.cpp \
    gui/QDaqErrorLog.cpp \
    gui/QDaqIde.cpp \
    gui/QDaqScriptEditor.cpp

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
    gui/jsedit.h \
    gui/QDaqObjectBrowser.h \
    gui/QDaqObjectController.h \
    gui/QDaqErrorLog.h \
    gui/QDaqIde.h \
    gui/QDaqScriptEditor.h

INCLUDEPATH += ./core ./gui ./daq

DISTFILES += \
    qdaq.pri \
    .gitignore

RESOURCES += \
    qdaq.qrc
