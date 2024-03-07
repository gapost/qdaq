#-------------------------------------------------
#
# Project created by QtCreator 2018-01-25T17:18:35
#
#-------------------------------------------------

QT       += gui widgets script

TARGET = qdaq
TEMPLATE = app
DESTDIR = $$OUT_PWD/../../bin

include(../../qdaq_version.pri)

SOURCES += main.cpp

HEADERS  +=

target.path = $$[QT_INSTALL_BINS]
INSTALLS += target

# RESOURCES += \
#    qdaq.qrc

RC_ICONS = qdaq_logo.ico

LIBS += -L$$OUT_PWD/../../bin/ -lQDaqCore
INCLUDEPATH += $$PWD/../../src/core
DEPENDPATH += $$PWD/../../src/core

LIBS += -L$$OUT_PWD/../../bin/ -lQDaqGui
INCLUDEPATH += $$PWD/../../src/gui $$PWD/../../src/gui/qconsolewidget/src
DEPENDPATH += $$PWD/../../src/gui
