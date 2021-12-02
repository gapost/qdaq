#-------------------------------------------------
#
# Project created by QtCreator 2018-01-09T23:05:54
#
#-------------------------------------------------

QT  += core gui widgets script scripttools uitools

TEMPLATE = lib
DEFINES += QDAQ_LIBRARY

# platform options
win32 {
    TARGET = libQDaqGui
    # Trick Qt to not add version major to the target dll name
    TARGET_EXT = .dll
    CONFIG(debug, debug|release) {
        DESTDIR = $$PWD/../../../bin-debug
    } else {
        DESTDIR = $$PWD/../../../bin-release
    }
}
unix {
    TARGET = QDaqGui
    DESTDIR = $$OUT_PWD/../../bin
}

SOURCES += \
    QDaqDelegates.cpp \
    QDaqConsole.cpp \
    QDaqObjectController.cpp \
    QDaqObjectBrowser.cpp \
    QDaqErrorLog.cpp \
    QDaqIde.cpp \
    QDaqScriptEditor.cpp \
    QDaqWindow.cpp \
    QDaqPlotWidget.cpp \
    QDaqLed.cpp \
    QDaqObjectModel.cpp \ 
    QDaqUi.cpp


HEADERS  += \
    QDaqDelegates.h \
    QDaqConsole.h \
    QDaqObjectBrowser.h \
    QDaqObjectController.h \
    QDaqErrorLog.h \
    QDaqIde.h \
    QDaqScriptEditor.h \
    QDaqWindow.h \
    QDaqPlotWidget.h \
    variantmanager.h \
    QDaqLed.h \
    QDaqObjectModel.h \ 
    QDaqUi.h


include(qconsolewidget/src/qconsolewidget.pri)
SOURCES += qconsolewidget/examples/scriptconsole/qscriptcompleter.cpp

include(qmatplotwidget/src/qmatplotwidget.pri)

## JSedit
SOURCES += 3rdparty/jsedit/jsedit.cpp
HEADERS += 3rdparty/jsedit/jsedit.h

## KLed
SOURCES += 3rdparty/kled/kled.cpp
HEADERS += 3rdparty/kled/kled.h


RESOURCES += \
    qdaqgui.qrc

unix {
    target.path = $$[QT_INSTALL_LIBS]
    headers.files  = $${HEADERS}
    headers.path   = $$[QT_INSTALL_HEADERS]/QDaq
    INSTALLS += headers target
}



win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../core/release/ -lQDaqCore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../core/debug/ -lQDaqCore
else:unix:!macx: LIBS += -L$$OUT_PWD/../../bin/ -lQDaqCore

INCLUDEPATH += $$PWD/../core
DEPENDPATH += $$PWD/../core

############# linux 3rd party libs ##############

unix {

    ######### Qwt ###############
    CONFIG += qwt

    ######### QtSolutions ###############
    CONFIG += qtpropertybrowser

    UBUNTU = $$system(cat /proc/version | grep -o Ubuntu)

    contains( UBUNTU, Ubuntu ) {
        INCLUDEPATH += /usr/include/qtpropertybrowser
        LIBS += -lqtpropertybrowser
    }

}

############## 3rd Party Libs for win32 ###############

win32 {
# Qwt-6
DEFINES += QWT_DLL
QWT_PATH = $$PWD/../3rdparty/Qwt-6.1.3-qt-5.9.2
LIBS += -L$$QWT_PATH/lib/
CONFIG(release, debug|release) {
    LIBS += -lqwt
} else {
    LIBS += -lqwtd
}
INCLUDEPATH += $$QWT_PATH/include
DEPENDPATH += $$QWT_PATH/include

######### QtSolutions ###############
QTSOLUTIONS_PATH = $$PWD/../3rdparty/qt-solutions/qtpropertybrowser
INCLUDEPATH += $$QTSOLUTIONS_PATH/src
LIBS += -L$$QTSOLUTIONS_PATH/lib
DEFINES += QT_QTPROPERTYBROWSER_IMPORT
CONFIG(debug, debug|release) {
        LIBS += -lQtSolutions_PropertyBrowser-headd
} else {
        LIBS += -lQtSolutions_PropertyBrowser-head
}

DEFINES += _CRT_SECURE_NO_WARNINGS

}
