#-------------------------------------------------
#
# Project created by QtCreator 2018-02-01T17:26:45
#
#-------------------------------------------------

QT       += widgets

TARGET = QDaqWidgets
TEMPLATE = lib

lessThan(QT_MAJOR_VERSION, 5): error("This project needs Qt5")

INCLUDEPATH += /usr/include/qt5/qwt
LIBS += -lqwt-qt5

DEFINES += QDAQ_WIDGETS_LIBRARY

SOURCES += \
    qled.cpp \
    QDaqPlotWidget.cpp

HEADERS +=\
    qled.h \
    qdaq_widgets_global.h \
    QDaqPlotWidget.h

unix {
    target.path = $$[QT_INSTALL_LIBS]
    headers.files  = $${HEADERS}
    headers.path   = $$[QT_INSTALL_HEADERS]
    INSTALLS += headers target
}


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../lib/release/ -lqdaq
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../lib/debug/ -lqdaq
else:unix: LIBS += -L$$OUT_PWD/../../lib/ -lqdaq

INCLUDEPATH += $$PWD/../../lib
DEPENDPATH += $$PWD/../../lib
