#-------------------------------------------------
#
# Project created by QtCreator 2018-01-09T23:05:54
#
#-------------------------------------------------

QT       += core gui widgets script network

lessThan(QT_MAJOR_VERSION, 5): error("This app needs Qt5")

include(qdaq.pri)

TARGET = qdaq
TEMPLATE = app

SOURCES += main.cpp\
    core/QDaqObject.cpp \
    core/QDaqRoot.cpp \
    core/QDaqH5Serialize.cpp \
    core/QDaqTypes.cpp

HEADERS  += \
    core/QDaqObject.h \
    core/QDaqGlobal.h \
    core/QDaqRoot.h \
    core/QDaqTypes.h

DISTFILES += \
    qdaq.pri
