set(_SOURCES 
    QDaqSession.cpp 
    QDaqObject.cpp 
    QDaqRoot.cpp 
    QDaqH5Serialize.cpp 
    QDaqTypes.cpp 
    QDaqJob.cpp 
    QDaqLogFile.cpp 
    QDaqChannel.cpp 
    QDaqDataBuffer.cpp 
    QDaqInterface.cpp 
    QDaqDevice.cpp 
    bytearrayclass.cpp 
    bytearrayprototype.cpp 
    QDaqFilter.cpp 
    qtimerthread.cpp 
    h5helper_v1_0.cpp 
    qdaqh5file.cpp 
    h5helper_v1_1.cpp 
    vectorclass.cpp 
    vectorprototype.cpp 
    QDaqScriptAPI.cpp 
    qsessiondelegate.cpp
)

set(_HEADERS  
    QDaqSession.h 
    QDaqObject.h 
    QDaqGlobal.h 
    QDaqRoot.h 
    QDaqTypes.h 
    QDaqJob.h 
    QDaqVector.h 
    math_util.h 
    QDaqLogFile.h 
    QDaqChannel.h 
    QDaqDataBuffer.h 
    QDaqInterface.h 
    QDaqDevice.h 
    bytearrayclass.h 
    bytearrayprototype.h 
    QDaqFilter.h 
    qtimerthread.h 
    h5helper_v1_0.h 
    qdaqh5file.h 
    h5helper_v1_1.h 
    vectorprototype.h 
    vectorclass.h 
    QDaqScriptAPI.h 
    qsessiondelegate.h
)

set(TARGET_NAME QDaqCore)

add_library(${TARGET_NAME} SHARED
    ${_SOURCES}
    ${_HEADERS}
    )

add_library(QDaq::Core ALIAS ${TARGET_NAME})

target_compile_definitions(${TARGET_NAME} PRIVATE GIT_VERSION="${PROJECT_VERSION}")

target_link_libraries(${TARGET_NAME}
    PRIVATE
    Qt5::Core
    Qt5::Widgets
    Qt5::Script
    Qt5::ScriptTools
    QtHDF5::QtHDF5
    muparser::muparser
    )

target_include_directories(${TARGET_NAME}
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/QDaq>
    )


# Install the library target and export targets file
install(TARGETS ${TARGET_NAME}
    EXPORT ${TARGET_NAME}Targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

