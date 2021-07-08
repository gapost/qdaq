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
    CONFIG(debug, debug|release) {
        DESTDIR = $$PWD/../../../bin-debug
    } else {
        DESTDIR = $$PWD/../../../bin-release
    }
}
unix {
    TARGET = QDaqFilters
    DESTDIR = $$OUT_PWD/../../bin
}


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

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../core/release/ -lQDaqCore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../core/debug/ -lQDaqCore
else:unix:!macx: LIBS += -L$$OUT_PWD/../../bin/ -lQDaqCore

INCLUDEPATH += $$PWD/../core
DEPENDPATH += $$PWD/../core


######### gsl ###############
unix:LIBS += -lgsl
win32 {
    GSL_PATH = $$PWD/../../../3rdparty/gsl-1.16
    INCLUDEPATH += $$GSL_PATH/include
    LIBS += -L$$GSL_PATH/lib/
    LIBS += -lgsl -lgslcblas
}
