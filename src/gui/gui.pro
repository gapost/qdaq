#-------------------------------------------------
#
# Project created by QtCreator 2018-01-09T23:05:54
#
#-------------------------------------------------

QT  += core gui widgets script scripttools uitools

TEMPLATE = lib
DEFINES += QDAQ_LIBRARY QCONSOLE_LIBRARY
TARGET = QDaqGui
# platform options
win32 {
    # Trick Qt to not add version major to the target dll name
    TARGET_EXT = .dll
}

DESTDIR = $$OUT_PWD/../../bin

include(../../qdaq_version.pri)

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

target.path = $$[QT_INSTALL_LIBS]
headers.files  = $${HEADERS}
headers.path   = $$[QT_INSTALL_HEADERS]/QDaq
INSTALLS += headers target

win32 {
    dlltarget.path = $$[QT_INSTALL_BINS]                                                                               
    INSTALLS += dlltarget
}

LIBS += -L$$OUT_PWD/../../bin/ -lQDaqCore

INCLUDEPATH += $$PWD/../core
DEPENDPATH += $$PWD/../core

############# 3rd party libs ##############

######### Qwt ###############
CONFIG += qwt
win32 {
    INCLUDEPATH += $$[QT_INSTALL_HEADERS]/qwt-qt5
}

######### QtSolutions ###############
CONFIG += qtpropertybrowser

unix {

    

    UBUNTU = $$system(cat /proc/version | grep -o Ubuntu)

    contains( UBUNTU, Ubuntu ) {
        INCLUDEPATH += /usr/include/qtpropertybrowser
        LIBS += -lqtpropertybrowser
    }

}

FORMS +=


