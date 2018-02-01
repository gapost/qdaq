#-------------------------------------------------
#
# Project created by QtCreator 2018-02-01T17:26:45
#
#-------------------------------------------------

QT       += widgets

TARGET = QDaqWidgets
TEMPLATE = lib

lessThan(QT_MAJOR_VERSION, 5): error("This project needs Qt5")


DEFINES += QDAQ_WIDGETS_LIBRARY

SOURCES += \
    qled.cpp

HEADERS +=\
    qled.h \
    qdaq_widgets_global.h

unix {
    target.path = $$[QT_INSTALL_LIBS]
    headers.files  = $${HEADERS}
    headers.path   = $$[QT_INSTALL_HEADERS]
    INSTALLS += headers target
}
