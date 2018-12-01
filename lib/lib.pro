#-------------------------------------------------
#
# Project created by QtCreator 2018-01-09T23:05:54
#
#-------------------------------------------------

QT       += core gui widgets script scripttools uitools testlib

lessThan(QT_MAJOR_VERSION, 5): error("This project needs Qt5")

include(../qdaq.pri)

# set a qmake variable
GIT_VERSION = "$$cat(../gitversion.txt)"

# Turns describe output like 0.1.5-42-g652c397 into "0.1.5.42.652c397"
GIT_VERSION ~= s/-/"."
GIT_VERSION ~= s/g/""

# Adding C preprocessor #DEFINE so we can use it in C++ code
# also here we want full version on every system so using GIT_VERSION
DEFINES += GIT_VERSION=\\\"$$GIT_VERSION\\\"

win32:TARGET = libQDaq
unix:TARGET = QDaq
# Trick Qt to not add version major to the target dll name
win32 { TARGET_EXT = .dll }
TEMPLATE = lib
DEFINES += QDAQ_LIBRARY

win32 {
CONFIG(debug, debug|release) {
    DESTDIR = $$PWD/../../bin-debug
} else {
    DESTDIR = $$PWD/../../bin-release
}
}

INCLUDEPATH += ./core ./gui ./daq

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
    gui/QDaqObjectController.cpp \
    gui/QDaqObjectBrowser.cpp \
    gui/QDaqErrorLog.cpp \
    gui/QDaqIde.cpp \
    gui/QDaqScriptEditor.cpp \
    gui/QDaqDelegates.cpp \
    gui/QDaqWindow.cpp \
    daq/QDaqInterface.cpp \
    daq/QDaqDevice.cpp \
    gui/QDaqPlotWidget.cpp \
    gui/QDaqLed.cpp \
    gui/QDaqObjectModel.cpp \
    core/bytearrayclass.cpp \
    core/bytearrayprototype.cpp \
    core/QDaqFilter.cpp \
    core/qtimerthread.cpp \
    core/h5helper_v1_0.cpp \
    core/qdaqh5file.cpp

HEADERS  += \
    core/QDaqSession.h \
    core/QDaqObject.h \
    core/QDaqGlobal.h \
    core/QDaqRoot.h \
    core/QDaqTypes.h \
    core/QDaqJob.h \
    core/math_util.h \
    gui/QConsoleWidget.h \
    gui/QDaqConsole.h \
    core/QDaqLogFile.h \
    core/QDaqChannel.h \
    core/QDaqDataBuffer.h \
    gui/QDaqObjectBrowser.h \
    gui/QDaqObjectController.h \
    gui/QDaqErrorLog.h \
    gui/QDaqIde.h \
    gui/QDaqScriptEditor.h \
    gui/QDaqDelegates.h \
    gui/QDaqWindow.h \
    daq/QDaqInterface.h \
    daq/QDaqDevice.h \
    QDaqBuffer \
    gui/QDaqPlotWidget.h \
    gui/variantmanager.h \
    gui/QDaqLed.h \
    gui/QDaqObjectModel.h \
    core/bytearrayclass.h \
    core/bytearrayprototype.h \
    core/QDaqFilter.h \
    core/qtimerthread.h \
    core/qdaqplugin.h \
    core/h5helper_v1_0.h \
    core/qdaqh5file.h


## JSedit
SOURCES += 3rdparty/jsedit/jsedit.cpp
HEADERS += 3rdparty/jsedit/jsedit.h

## KLed
SOURCES += 3rdparty/kled/kled.cpp
HEADERS += 3rdparty/kled/kled.h

DISTFILES += \
    qdaq.pri \
    .gitignore \
    ../TODO \
    ../qdaq.dox \
    qdaq_modules.dox \
    qdaq_script_api.dox \
    ../makeArchive.sh \
    ../makeGitversion.sh

RESOURCES += \
    qdaq.qrc

unix {
    target.path = $$[QT_INSTALL_LIBS]
    headers.files  = $${HEADERS}
    headers.path   = $$[QT_INSTALL_HEADERS]/QDaq
    INSTALLS += headers target
}
