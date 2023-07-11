#-------------------------------------------------
#
# Project created by QtCreator 2018-01-09T23:05:54
#
#-------------------------------------------------

QT       += core script scripttools

TEMPLATE = lib
DEFINES += QDAQ_LIBRARY

# platform options
TARGET = QDaqCore
win32 {
    # Trick Qt to not add version major to the target dll name
    TARGET_EXT = .dll
}
DESTDIR = $$OUT_PWD/../../bin

include(../../qdaq_version.pri)

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



target.path = $$[QT_INSTALL_LIBS]
headers.files  = $${HEADERS}
headers.path   = $$[QT_INSTALL_HEADERS]/QDaq
INSTALLS += headers target

win32 {
    dlltarget.path = $$[QT_INSTALL_BINS]                                                                               
    INSTALLS += dlltarget
}

############# 3rd party libs ##############


############## HDF5 ##############
LIBS += -lhdf5 -lhdf5_cpp

############## muParser ##############
LIBS += -lmuparser
