QT += designer
CONFIG      += plugin debug_and_release

TARGET      = $$qtLibraryTarget(qdaqwidgetsplugin)
# Trick Qt to not add version major to the target dll name
win32 { TARGET_EXT = .dll }
TEMPLATE    = lib

HEADERS     = qledplugin.h qdaqplotwidgetplugin.h qdaqwidgetsplugin.h \
    qdaqconsoletabplugin.h
SOURCES     = qledplugin.cpp qdaqplotwidgetplugin.cpp qdaqwidgetsplugin.cpp \
    qdaqconsoletabplugin.cpp
RESOURCES   = icons.qrc
LIBS        += -L.

lessThan(QT_MAJOR_VERSION, 5): error("This project needs Qt5")

include(../qdaq.pri)
#include(../gitversion.pri)

unix {
  target.path = $$[QT_INSTALL_PLUGINS]/designer
  INSTALLS    += target
}

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../lib/release/ -lQDaq
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../lib/debug/ -lQDaq
else:unix: LIBS += -L$$OUT_PWD/../lib/ -lQDaq

INCLUDEPATH += $$PWD/../lib $$PWD/../lib/core $$PWD/../lib/gui $$PWD/../lib/daq
DEPENDPATH += $$PWD/../lib $$PWD/../lib/core $$PWD/../lib/gui $$PWD/../lib/daq
