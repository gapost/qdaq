# Configure version.cpp
# all @###@ codes are replaced except for @GIT_TAG@
# which is configured during build
set(GIT_TAG "@GIT_TAG@")
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/qdaqversion.cpp.in
    ${CMAKE_CURRENT_BINARY_DIR}/qdaqversion1.cpp
)

# Update the GIT_TAG in version.cpp
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/qdaqversion.cpp
    COMMAND ${CMAKE_COMMAND}
    -DRUN_BUILD_CONFIG_FILE=1
    -DFILE_IN=${CMAKE_CURRENT_BINARY_DIR}/qdaqversion1.cpp
    -DFILE_OUT=${CMAKE_CURRENT_BINARY_DIR}/qdaqversion.cpp
    -DFILE_CONTENT=${CMAKE_SOURCE_DIR}/gitversion.txt
    -DVAR_NAME="GIT_TAG"
    -P ${CMAKE_SOURCE_DIR}/cmake/BuildConfigFile.cmake
    DEPENDS 
        ${CMAKE_SOURCE_DIR}/gitversion.txt
        qdaqversion.cpp.in
)

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
    ${CMAKE_CURRENT_BINARY_DIR}/qdaqversion.cpp
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

if(WIN32)
    target_compile_definitions(${TARGET_NAME} PRIVATE QDAQ_LIBRARY)
endif()

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
if(WIN32)
    install(TARGETS ${TARGET_NAME}
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}
    )
else()
    install(TARGETS ${TARGET_NAME}
        EXPORT ${TARGET_NAME}Targets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
endif()

