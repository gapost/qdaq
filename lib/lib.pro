#-------------------------------------------------
#
# Project created by QtCreator 2018-01-09T23:05:54
#
#-------------------------------------------------

QT       += core gui widgets script scripttools network uitools serialport

lessThan(QT_MAJOR_VERSION, 5): error("This project needs Qt5")

include(../qdaq.pri)

TARGET = QDaq
TEMPLATE = lib

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
    daq/tcp_socket.cpp \
    daq/QDaqInterface.cpp \
    daq/QDaqDevice.cpp \
    gui/QDaqPlotWidget.cpp \
    gui/QDaqLed.cpp \
    gui/QDaqObjectModel.cpp

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
    gui/QDaqObjectBrowser.h \
    gui/QDaqObjectController.h \
    gui/QDaqErrorLog.h \
    gui/QDaqIde.h \
    gui/QDaqScriptEditor.h \
    gui/QDaqDelegates.h \
    gui/QDaqWindow.h \
    daq/tcp_socket.h \
    daq/QDaqInterface.h \
    daq/QDaqDevice.h \
    QDaqBuffer \
    gui/QDaqPlotWidget.h \
    gui/variantmanager.h \
    gui/QDaqLed.h \
    gui/QDaqObjectModel.h


## JSedit
SOURCES += 3rdparty/jsedit/jsedit.cpp
HEADERS += 3rdparty/jsedit/jsedit.h

## KLed
SOURCES += 3rdparty/kled/kled.cpp
HEADERS += 3rdparty/kled/kled.h

DISTFILES += \
    qdaq.pri \
    .gitignore \
    ../TODO

RESOURCES += \
    qdaq.qrc

unix {
    target.path = $$[QT_INSTALL_LIBS]
    headers.files  = $${HEADERS}
    headers.path   = $$[QT_INSTALL_HEADERS]/QDaq
    INSTALLS += headers target
}

## QtPropertyBrowser Lib

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../qtpropertybrowser/release/ -lqtpropertybrowser
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../qtpropertybrowser/debug/ -lqtpropertybrowser
else:unix: LIBS += -L$$OUT_PWD/../qtpropertybrowser/ -lqtpropertybrowser

INCLUDEPATH += $$PWD/../qtpropertybrowser $$PWD/3rdparty/qt-solutions/qtpropertybrowser/src
DEPENDPATH += $$PWD/../qtpropertybrowser

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../qtpropertybrowser/release/libqtpropertybrowser.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../qtpropertybrowser/debug/libqtpropertybrowser.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../qtpropertybrowser/release/qtpropertybrowser.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../qtpropertybrowser/debug/qtpropertybrowser.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../qtpropertybrowser/libqtpropertybrowser.a
