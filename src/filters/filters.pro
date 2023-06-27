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
win32 {
    TARGET = libQDaqFilters
    # Trick Qt to not add version major to the target dll name
    TARGET_EXT = .dll
}
unix {
    TARGET = QDaqFilters
}
DESTDIR = $$OUT_PWD/../../bin


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

unix {
    target.path = $$[QT_INSTALL_LIBS]
    headers.files  = $${HEADERS}
    headers.path   = $$[QT_INSTALL_HEADERS]/QDaq
    INSTALLS += headers target
}

## Add module core

LIBS += -L$$OUT_PWD/../../bin/ -lQDaqCore

INCLUDEPATH += $$PWD/../core
DEPENDPATH += $$PWD/../core


######### gsl ###############
LIBS += -lgsl
