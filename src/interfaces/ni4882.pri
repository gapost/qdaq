win32 {
    SOURCES += nigpib.cpp
    HEADERS += nigpib.h
}

win32 {
    ############## NI-488.2 ##############
    NIPATH = $$PWD/../../../3rdparty/ni4882-1.6
    LIBS += $$NIPATH/gpib-32.obj
    INCLUDEPATH += $$NIPATH
}