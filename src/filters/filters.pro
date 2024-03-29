#-------------------------------------------------
#
# Project created by QtCreator 2020-09-11T14:59:43
#
#-------------------------------------------------

QT       += script
QT       -= gui

TEMPLATE = lib
DEFINES += QDAQ_LIBRARY

# platform options
TARGET = QDaqFilters
win32 {
    # Trick Qt to not add version major to the target dll name
    TARGET_EXT = .dll
}
DESTDIR = $$OUT_PWD/../../bin

include(../../qdaq_version.pri)


SOURCES +=  \
    qdaqfopdt.cpp \
    qdaqinterpolator.cpp \
    qdaqlinearcorrelator.cpp \
    qdaqpid.cpp \
    qdaqfilters.cpp

HEADERS += \
    qdaqfopdt.h \
    linefit.h \
    qdaqlinearcorrelator.h \
    qdaqinterpolator.h \
    qdaqpid.h \
    relaytuner.h \
    isa_pid.h \
    qdaqfilters.h

target.path = $$[QT_INSTALL_LIBS]
headers.files  = $${HEADERS}
headers.path   = $$[QT_INSTALL_HEADERS]/QDaq
INSTALLS += headers target

win32 {
    dlltarget.path = $$[QT_INSTALL_BINS]                                                                               
    INSTALLS += dlltarget
}

## Add module core

LIBS += -L$$OUT_PWD/../../bin/ -lQDaqCore

INCLUDEPATH += $$PWD/../core
DEPENDPATH += $$PWD/../core


######### gsl ###############
LIBS += -lgsl
