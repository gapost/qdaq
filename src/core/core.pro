#-------------------------------------------------
#
# Project created by QtCreator 2018-01-09T23:05:54
#
#-------------------------------------------------

QT       += core script scripttools

lessThan(QT_MAJOR_VERSION, 5): error("This project needs Qt5")

# set a qmake variable
GIT_VERSION = "$$cat(../../gitversion.txt)"

# Turns describe output like 0.1.5-42-g652c397 into "0.1.5.42.652c397"
GIT_VERSION ~= s/-/"."
GIT_VERSION ~= s/g/""

# Adding C preprocessor #DEFINE so we can use it in C++ code
# also here we want full version on every system so using GIT_VERSION
DEFINES += GIT_VERSION=\\\"$$GIT_VERSION\\\"

win32:TARGET = libQDaqCore
unix:TARGET = QDaqCore
# Trick Qt to not add version major to the target dll name
win32 { TARGET_EXT = .dll }
TEMPLATE = lib
DEFINES += QDAQ_LIBRARY

win32 {
    CONFIG(debug, debug|release) {
        DESTDIR = $$PWD/../../../bin-debug
    } else {
        DESTDIR = $$PWD/../../../bin-release
    }
}

SOURCES += \
    QDaqSession.cpp \
    QDaqObject.cpp \
    QDaqRoot.cpp \
    QDaqH5Serialize.cpp \
    QDaqTypes.cpp \
    QDaqJob.cpp \
    QDaqLogFile.cpp \
    QDaqChannel.cpp \
    QDaqDataBuffer.cpp \
    QDaqInterface.cpp \
    QDaqDevice.cpp \
    bytearrayclass.cpp \
    bytearrayprototype.cpp \
    QDaqFilter.cpp \
    qtimerthread.cpp \
    h5helper_v1_0.cpp \
    qdaqh5file.cpp \
    h5helper_v1_1.cpp \
    vectorclass.cpp \
    vectorprototype.cpp \
    QDaqScriptAPI.cpp \
    qsessiondelegate.cpp

HEADERS  += \
    QDaqSession.h \
    QDaqObject.h \
    QDaqGlobal.h \
    QDaqRoot.h \
    QDaqTypes.h \
    QDaqJob.h \
    QDaqVector.h \
    math_util.h \
    QDaqLogFile.h \
    QDaqChannel.h \
    QDaqDataBuffer.h \
    QDaqInterface.h \
    QDaqDevice.h \
    bytearrayclass.h \
    bytearrayprototype.h \
    QDaqFilter.h \
    qtimerthread.h \
    h5helper_v1_0.h \
    qdaqh5file.h \
    h5helper_v1_1.h \
    vectorprototype.h \
    vectorclass.h \
    QDaqScriptAPI.h \
    qsessiondelegate.h


DISTFILES += \
    ../../.gitignore \
    ../../TODO \
    ../../makeArchive.sh \
    ../../makeGitversion.sh \
    ../../.gitmodules \
    ../../gitversion.txt


unix {
    target.path = $$[QT_INSTALL_LIBS]
    headers.files  = $${HEADERS}
    headers.path   = $$[QT_INSTALL_HEADERS]/QDaq
    INSTALLS += headers target
}

############# linux 3rd party libs ##############

unix {

    ############## HDF5 ##############
    LIBS += -lhdf5 -lhdf5_cpp

    ############## muParser ##############
    LIBS += -lmuparser

}

############## 3rd Party Libs for win32 ###############

win32 {

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

DEFINES += _CRT_SECURE_NO_WARNINGS

}
