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

    ######### libmodbus #############
    LIBS += -lmodbus
    INCLUDEPATH += /usr/include/modbus

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
QWT_PATH = $$PWD/../3rdparty/Qwt-6.1.3-qt-5.6.3-msvc2015
LIBS += -L$$QWT_PATH/lib/
CONFIG(release, debug|release) {
    LIBS += -lqwt
} else {
    LIBS += -lqwtd
}
INCLUDEPATH += $$QWT_PATH/include
DEPENDPATH += $$QWT_PATH/include

############## HDF5 ##############
HDF5_PATH = $$PWD/../3rdparty/hdf5-1.8.20-msvc2015
DEFINES += _HDF5USEDLL_ HDF5CPP_USEDLL H5_BUILT_AS_DYNAMIC_LIB
INCLUDEPATH += $$HDF5_PATH/include
LIBS += -L$$HDF5_PATH/lib -lhdf5 -lhdf5_cpp


############## muParser ##############
MUPARSER_PATH = $$PWD/../3rdparty/muparser-2.2.5
INCLUDEPATH += $$MUPARSER_PATH/include
LIBS += -L$$MUPARSER_PATH/lib/
    CONFIG(debug, debug|release) {
        LIBS += -lmuparserd
    } else {
        LIBS += -lmuparser
    }

############# libmodbus ##############
LIBMODBUS_PATH = $$PWD/../3rdparty/libmodbus-3.1.4-msvc2015
INCLUDEPATH += $$LIBMODBUS_PATH/include
LIBS += -L$$LIBMODBUS_PATH/lib -lmodbus

######### QtSolutions ###############
QTSOLUTIONS_PATH = $$PWD/../3rdparty/qtpropertybrowser
INCLUDEPATH += $$QTSOLUTIONS_PATH/src
LIBS += -L$$QTSOLUTIONS_PATH/lib
DEFINES += QT_QTPROPERTYBROWSER_IMPORT
    CONFIG(debug, debug|release) {
        LIBS += -lQtSolutions_PropertyBrowser-headd
    } else {
        LIBS += -lQtSolutions_PropertyBrowser-head
    }

######## Win32 Libs ##########
LIBS += winmm.lib user32.lib ws2_32.lib

DEFINES += _CRT_SECURE_NO_WARNINGS

DEFINES += __finite=isfinite
}



