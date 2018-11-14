############# linux 3rd party libs ##############

unix {
    ########## system libs ##########
    LIBS += -lrt -lpthread

    ########## ATLAS ################
    centos6 { LIBS += -lcblas -latlas -lm }
    else { LIBS += -lm }

    ############## GSL ##############
    LIBS += -lgsl

    ############## HDF5 ##############
    LIBS += -lhdf5 -lhdf5_cpp

    ############## muParser ##############
    LIBS += -lmuparser

    ######### comedi #############
    LIBS += -lcomedi

    ######### Qwt ###############
    INCLUDEPATH += /usr/include/qt5/qwt
    LIBS += -lqwt-qt5

    ######### QtSolutions ###############
    INCLUDEPATH += /usr/include/qt5/QtSolutions
    LIBS += -lQtSolutions_PropertyBrowser
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

############## HDF5 ##############
win32: LIBS += -L$$PWD/../3rdparty/HDF5/1.8.20/lib/ -lhdf5 -lhdf5_cpp
INCLUDEPATH += $$PWD/../3rdparty/HDF5/1.8.20/include
DEPENDPATH += $$PWD/../3rdparty/HDF5/1.8.20/include
DEFINES += H5_BUILT_AS_DYNAMIC_LIB

############## muParser ##############
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../3rdparty/muparser-2.2.5/lib/ -lmuparser
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../3rdparty/muparser-2.2.5/lib/ -lmuparserd

INCLUDEPATH += $$PWD/../3rdparty/muparser-2.2.5/include
DEPENDPATH += $$PWD/../3rdparty/muparser-2.2.5/include

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../3rdparty/muparser-2.2.5/lib/libmuparser.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../3rdparty/muparser-2.2.5/lib/libmuparserd.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../3rdparty/muparser-2.2.5/lib/muparser.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../3rdparty/muparser-2.2.5/lib/muparserd.lib


######### QtSolutions ###############
QTSOLUTIONS_PATH = $$PWD/../3rdparty/qt-solutions/qtpropertybrowser
INCLUDEPATH += $$QTSOLUTIONS_PATH/src
LIBS += -L$$QTSOLUTIONS_PATH/lib
DEFINES += QT_QTPROPERTYBROWSER_IMPORT
CONFIG(debug, debug|release) {
	LIBS += -lQtSolutions_PropertyBrowser-headd
} else {
	LIBS += -lQtSolutions_PropertyBrowser-head
}

######## Win32 Libs ##########
# LIBS += winmm.lib user32.lib ws2_32.lib

DEFINES += _CRT_SECURE_NO_WARNINGS

}











