CONFIG      += plugin debug_and_release

TARGET      = $$qtLibraryTarget(qdaqwidgetsplugin)
TEMPLATE    = lib

HEADERS     = qledplugin.h qdaqplotwidgetplugin.h qdaqwidgetsplugin.h
SOURCES     = qledplugin.cpp qdaqplotwidgetplugin.cpp qdaqwidgetsplugin.cpp
RESOURCES   = icons.qrc
LIBS        += -L.

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += designer
} else {
    CONFIG += designer
}

target.path = $$[QT_INSTALL_PLUGINS]/designer
INSTALLS    += target




win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../src/release/ -lQDaqWidgets
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../src/debug/ -lQDaqWidgets
else:unix: LIBS += -L$$OUT_PWD/../src/ -lQDaqWidgets

INCLUDEPATH += $$PWD/../src
DEPENDPATH += $$PWD/../src

INCLUDEPATH += /usr/include/qt5/qwt
LIBS += -lqwt-qt5


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../lib/release/ -lQDaq
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../lib/debug/ -lQDaq
else:unix: LIBS += -L$$OUT_PWD/../../lib/ -lQDaq

INCLUDEPATH += $$PWD/../../lib
DEPENDPATH += $$PWD/../../lib
