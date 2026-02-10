set(_SOURCES 
    qdaqfopdt.cpp 
    qdaqinterpolator.cpp 
    qdaqlinearcorrelator.cpp 
    qdaqpid.cpp 
    qdaqfilters.cpp
)

set(_HEADERS  
    qdaqfopdt.h 
    linefit.h 
    qdaqlinearcorrelator.h 
    qdaqinterpolator.h 
    qdaqpid.h 
    relaytuner.h 
    isa_pid.h 
    qdaqfilters.h
)

add_library(QDaqFilters SHARED
    ${_SOURCES}
    ${_HEADERS}
    )

add_library(QDaq::Filters ALIAS QDaqFilters)

target_link_libraries(QDaqFilters
    PRIVATE
    Qt5::Core
    Qt5::Script
    QDaq::Core
    PkgConfig::GSL
    )

target_include_directories(QDaqFilters
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/QDaqFilters>
    )

# -----------------------
# Installation and config
# -----------------------
include(CMakePackageConfigHelpers)

# Install the library target and export targets file
install(TARGETS QDaqFilters
    EXPORT QDaqFiltersTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/QDaqFilters
)

# Install public headers collected in INSTALL_HEADERS
install(FILES ${INSTALL_HEADERS}
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/QDaqFilters
)

# location where CMake config files will be installed (relocatable)
set(CONFIG_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/QDaqFilters")

# Expose QT major version for the package config template
set(PACKAGE_QT_MAJOR ${QT_VERSION_MAJOR})

# Install the exported targets so downstream projects can use target names
install(EXPORT QDaqFiltersTargets
    FILE QDaqFiltersTargets.cmake
    NAMESPACE QDaq::
    DESTINATION ${CONFIG_INSTALL_DIR}
)

# Generate and install package config + version files
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/QDaqFiltersConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY AnyNewerVersion
)

# configure_package_config_file(
#   "${CMAKE_CURRENT_SOURCE_DIR}/../cmake/QDaqFiltersConfig.cmake.in"
#   "${CMAKE_CURRENT_BINARY_DIR}/QDaqFiltersConfig.cmake"
#   INSTALL_DESTINATION "${CONFIG_INSTALL_DIR}"
# )

install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/QDaqFiltersConfig.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/QDaqFiltersConfigVersion.cmake"
  DESTINATION "${CONFIG_INSTALL_DIR}")