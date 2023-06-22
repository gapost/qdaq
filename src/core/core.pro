#-------------------------------------------------
#
# Project created by QtCreator 2018-01-09T23:05:54
#
#-------------------------------------------------

QT       += core script scripttools

TEMPLATE = lib
DEFINES += QDAQ_LIBRARY

# platform options
win32 {
    TARGET = libQDaqCore
    # Trick Qt to not add version major to the target dll name
    TARGET_EXT = .dll
    CONFIG(debug, debug|release) {
        DESTDIR = $$PWD/../../../bin-debug
    } else {
        DESTDIR = $$PWD/../../../bin-release
    }
}
unix {
    TARGET = QDaqCore
    DESTDIR = $$OUT_PWD/../../bin
}


# set a qmake variable
GIT_VERSION = "$$cat(../../gitversion.txt)"

# Turns describe output like 0.1.5-42-g652c397 into "0.1.5.42.652c397"
GIT_VERSION ~= s/-/"."
GIT_VERSION ~= s/g/""

# Adding C preprocessor #DEFINE so we can use it in C++ code
# also here we want full version on every system so using GIT_VERSION
DEFINES += GIT_VERSION=\\\"$$GIT_VERSION\\\"

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

include(qthdf5/src/qthdf5/qthdf5.pri)

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

############# 3rd party libs ##############


############## HDF5 ##############
LIBS += -lhdf5 -lhdf5_cpp

############## muParser ##############
LIBS += -lmuparser
