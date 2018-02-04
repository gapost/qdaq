QT += designer
CONFIG      += plugin debug_and_release

TARGET      = $$qtLibraryTarget(qdaqwidgetsplugin)
TEMPLATE    = lib

HEADERS     = qledplugin.h qdaqplotwidgetplugin.h qdaqwidgetsplugin.h
SOURCES     = qledplugin.cpp qdaqplotwidgetplugin.cpp qdaqwidgetsplugin.cpp
RESOURCES   = icons.qrc
LIBS        += -L.

lessThan(QT_MAJOR_VERSION, 5): error("This project needs Qt5")

include(../qdaq.pri)

target.path = $$[QT_INSTALL_PLUGINS]/designer
INSTALLS    += target


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../lib/release/ -lQDaq
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../lib/debug/ -lQDaq
else:unix: LIBS += -L$$OUT_PWD/../lib/ -lQDaq

INCLUDEPATH += $$PWD/../lib $$PWD/../lib/core $$PWD/../lib/gui $$PWD/../lib/daq
DEPENDPATH += $$PWD/../lib $$PWD/../lib/core $$PWD/../lib/gui $$PWD/../lib/daq
