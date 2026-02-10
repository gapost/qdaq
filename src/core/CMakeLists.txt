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

add_library(QDaqCore SHARED
    ${_SOURCES}
    ${_HEADERS}
    )

add_library(QDaq::Core ALIAS QDaqCore)

target_compile_definitions(QDaqCore PRIVATE GIT_VERSION="${PROJECT_VERSION}")

target_link_libraries(QDaqCore
    PRIVATE
    Qt5::Core
    Qt5::Widgets
    Qt5::Script
    Qt5::ScriptTools
    QtHDF5::QtHDF5
    muparser::muparser
    )

target_include_directories(QDaqCore
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/QDaqCore>
    )

# -----------------------
# Installation and config
# -----------------------
include(CMakePackageConfigHelpers)

# Install the library target and export targets file
install(TARGETS QDaqCore
    EXPORT QDaqCoreTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/QDaqCore
)

# Install public headers collected in INSTALL_HEADERS
install(FILES ${INSTALL_HEADERS}
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/QDaqCore
)

# location where CMake config files will be installed (relocatable)
set(CONFIG_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/QDaqCore")

# Expose QT major version for the package config template
set(PACKAGE_QT_MAJOR ${QT_VERSION_MAJOR})

# Install the exported targets so downstream projects can use target names
install(EXPORT QDaqCoreTargets
    FILE QDaqCoreTargets.cmake
    NAMESPACE QDaq::
    DESTINATION ${CONFIG_INSTALL_DIR}
)

# Generate and install package config + version files
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/QDaqCoreConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY AnyNewerVersion
)

# configure_package_config_file(
#   "${CMAKE_CURRENT_SOURCE_DIR}/../cmake/QDaqCoreConfig.cmake.in"
#   "${CMAKE_CURRENT_BINARY_DIR}/QDaqCoreConfig.cmake"
#   INSTALL_DESTINATION "${CONFIG_INSTALL_DIR}"
# )

install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/QDaqCoreConfig.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/QDaqCoreConfigVersion.cmake"
  DESTINATION "${CONFIG_INSTALL_DIR}")