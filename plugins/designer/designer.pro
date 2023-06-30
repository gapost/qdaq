QT += designer
CONFIG      += plugin debug_and_release

TEMPLATE    = lib
TARGET = $$qtLibraryTarget(qdaqwidgetsplugin)
# platform options
win32 {
    # Trick Qt to not add version major to the target dll name
    TARGET_EXT = .dll
}
DESTDIR = $$OUT_PWD/../../bin/designer


HEADERS     = qledplugin.h qdaqplotwidgetplugin.h qdaqwidgetsplugin.h \
    qdaqconsoletabplugin.h
SOURCES     = qledplugin.cpp qdaqplotwidgetplugin.cpp qdaqwidgetsplugin.cpp \
    qdaqconsoletabplugin.cpp
RESOURCES   = icons.qrc

target.path = $$[QT_INSTALL_PLUGINS]/designer
INSTALLS    += target


## Add modules core & gui

LIBS += -L$$OUT_PWD/../../bin/ -lQDaqCore
INCLUDEPATH += $$PWD/../../src/core
DEPENDPATH += $$PWD/../../src/core

LIBS += -L$$OUT_PWD/../../bin/ -lQDaqGui
INCLUDEPATH += $$PWD/../../src/gui \
               $$PWD/../../src/gui/qconsolewidget/src \
               $$PWD/../../src/gui/qmatplotwidget/src
DEPENDPATH += $$PWD/../../src/gui

############# linux 3rd party libs ##############

######### Qwt ###############
CONFIG += qwt

