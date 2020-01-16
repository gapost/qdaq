#-------------------------------------------------
#
# Project created by QtCreator 2018-05-24T10:40:56
#
#-------------------------------------------------

QT       += script
QT       -= gui
CONFIG   += plugin


INCLUDEPATH += $$PWD/../../lib/core
DEPENDPATH += $$PWD/../../lib/core

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../bin-release/ -llibQDaq
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../bin-debug/ -llibQDaq
else:unix: LIBS += -L$$OUT_PWD/../../lib/ -lQDaq

######### gsl ###############
unix:LIBS += -lgsl
win32 {
    GSL_PATH = $$PWD/../../../3rdparty/gsl-1.16
    INCLUDEPATH += $$GSL_PATH/include
    LIBS += -L$$GSL_PATH/lib/
    LIBS += -lgsl -lgslcblas
}


TARGET = $$qtLibraryTarget(qdaqfilters)
TEMPLATE = lib

win32:CONFIG(release, debug|release): DESTDIR = $$PWD/../../../bin-release/plugins
else:win32:CONFIG(debug, debug|release): DESTDIR = $$PWD/../../../bin-debug/plugins
else:unix: DESTDIR = ../../qdaq/plugins

DEFINES += FILTERS_LIBRARY

SOURCES += filterfactory.cpp \
    qdaqfopdt.cpp \
    qdaqinterpolator.cpp \
    qdaqlinearcorrelator.cpp \
    qdaqpid.cpp

HEADERS += filterfactory.h\
        filters_global.h \
    qdaqfopdt.h \
    linefit.h \
    qdaqlinearcorrelator.h \
    qdaqinterpolator.h \
    qdaqpid.h \
    relaytuner.h \
    isa_pid.h

unix {
    target.path = $$[QT_INSTALL_PLUGINS]/qdaq
    INSTALLS += target
}

DISTFILES += \
    filters.json






