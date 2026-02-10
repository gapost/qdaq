set(_SOURCES 
    QDaqDelegates.cpp 
    QDaqConsole.cpp 
    QDaqObjectController.cpp 
    QDaqObjectBrowser.cpp 
    QDaqErrorLog.cpp 
    QDaqIde.cpp 
    QDaqScriptEditor.cpp 
    QDaqWindow.cpp 
    QDaqPlotWidget.cpp 
    QDaqLed.cpp 
    QDaqObjectModel.cpp 
    QDaqUi.cpp
)

set(_HEADERS  
    QDaqDelegates.h 
    QDaqConsole.h 
    QDaqObjectBrowser.h 
    QDaqObjectController.h 
    QDaqErrorLog.h 
    QDaqIde.h 
    QDaqScriptEditor.h 
    QDaqWindow.h 
    QDaqPlotWidget.h 
    variantmanager.h 
    QDaqLed.h 
    QDaqObjectModel.h 
    QDaqUi.h
)

add_library(QDaqGui SHARED
    ${_SOURCES}
    ${_HEADERS}
    )

add_library(QDaq::Gui ALIAS QDaqGui)

target_compile_definitions(QDaqGui PRIVATE GIT_VERSION="${PROJECT_VERSION}")

target_link_libraries(QDaqGui
    PRIVATE
    Qt5::Core
    Qt5::Widgets
    Qt5::Script
    Qt5::ScriptTools
    QtHDF5::QtHDF5
    QDaq::Core
    QMatPlotWidget::QMatPlotWidget
    QConsoleWidget::QConsoleWidget
    Qt5PropertyBrowser::Qt5PropertyBrowser
    )

target_include_directories(QDaqGui
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/QDaqGui>
    )

# -----------------------
# Installation and config
# -----------------------
include(CMakePackageConfigHelpers)

# Install the library target and export targets file
install(TARGETS QDaqGui
    EXPORT QDaqGuiTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/QDaqGui
)

# Install public headers collected in INSTALL_HEADERS
install(FILES ${INSTALL_HEADERS}
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/QDaqGui
)

# location where CMake config files will be installed (relocatable)
set(CONFIG_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/QDaqGui")

# Expose QT major version for the package config template
set(PACKAGE_QT_MAJOR ${QT_VERSION_MAJOR})

# Install the exported targets so downstream projects can use target names
install(EXPORT QDaqGuiTargets
    FILE QDaqGuiTargets.cmake
    NAMESPACE QDaq::
    DESTINATION ${CONFIG_INSTALL_DIR}
)

# Generate and install package config + version files
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/QDaqGuiConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY AnyNewerVersion
)

# configure_package_config_file(
#   "${CMAKE_CURRENT_SOURCE_DIR}/../cmake/QDaqGuiConfig.cmake.in"
#   "${CMAKE_CURRENT_BINARY_DIR}/QDaqGuiConfig.cmake"
#   INSTALL_DESTINATION "${CONFIG_INSTALL_DIR}"
# )

install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/QDaqGuiConfig.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/QDaqGuiConfigVersion.cmake"
  DESTINATION "${CONFIG_INSTALL_DIR}")