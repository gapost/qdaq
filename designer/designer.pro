QT += designer
CONFIG      += plugin debug_and_release

lessThan(QT_MAJOR_VERSION, 5): error("This project needs Qt5")

TARGET      = $$qtLibraryTarget(qdaqwidgetsplugin)
# Trick Qt to not add version major to the target dll name
win32 { TARGET_EXT = .dll }
TEMPLATE    = lib

HEADERS     = qledplugin.h qdaqplotwidgetplugin.h qdaqwidgetsplugin.h \
    qdaqconsoletabplugin.h
SOURCES     = qledplugin.cpp qdaqplotwidgetplugin.cpp qdaqwidgetsplugin.cpp \
    qdaqconsoletabplugin.cpp
RESOURCES   = icons.qrc

unix {
  target.path = $$[QT_INSTALL_PLUGINS]/designer
  INSTALLS    += target
}


## Add modules core & gui

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../core/release/ -lQDaqCore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../core/debug/ -lQDaqCore
else:unix:!macx: LIBS += -L$$OUT_PWD/../core/ -lQDaqCore

INCLUDEPATH += $$PWD/../core
DEPENDPATH += $$PWD/../core

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../gui/release/ -lQDaqGui
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../gui/debug/ -lQDaqGui
else:unix:!macx: LIBS += -L$$OUT_PWD/../gui/ -lQDaqGui

INCLUDEPATH += $$PWD/../gui $$PWD/../gui/qconsolewidget/src
DEPENDPATH += $$PWD/../gui

############# linux 3rd party libs ##############

unix {
    ######### Qwt ###############
    CONFIG += qwt
}

############## 3rd Party Libs for win32 ###############

win32 {
# Qwt-6
DEFINES += QWT_DLL
QWT_PATH = $$PWD/../3rdparty/Qwt-6.1.3-qt-5.9.2
LIBS += -L$$QWT_PATH/lib/
CONFIG(release, debug|release) {
    LIBS += -lqwt
} else {
    LIBS += -lqwtd
}
INCLUDEPATH += $$QWT_PATH/include
DEPENDPATH += $$QWT_PATH/include


DEFINES += _CRT_SECURE_NO_WARNINGS

}
