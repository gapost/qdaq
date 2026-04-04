cmake_minimum_required(VERSION 3.16)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

project(QDaq 
  VERSION 0.3.6 
  LANGUAGES CXX
  DESCRIPTION "Qt-based Data Acquisition"
  HOMEPAGE_URL https://gitlab.com/qdaq/qdaq
)

string(TIMESTAMP CMAKE_BUILD_TIME UTC)

# add our custom cmake modules directory 
list(APPEND CMAKE_MODULE_PATH
     "${CMAKE_SOURCE_DIR}/cmake"
)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Option to build the examples
option(QDAQ_BUILD_EXAMPLES "Build the examples" OFF)

# Default install prefix (if not set by user)
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  if((CMAKE_SYSTEM_NAME STREQUAL "Windows") AND (NOT CMAKE_CROSSCOMPILING))
    set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/${PROJECT_NAME}" CACHE PATH "Default install path" FORCE)
  else()
    set(CMAKE_INSTALL_PREFIX "$ENV{HOME}/.local" CACHE PATH "Default install path" FORCE)
  endif()
endif()

# Install folder support
include(GNUInstallDirs)

# use install rpath, no need to use LD_LIBRARY_PATH 
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_FULL_LIBDIR}")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH OFF)

# Find dependencies.
find_package(Qt5 REQUIRED COMPONENTS 
  Core 
  Widgets 
  Network 
  Script 
  ScriptTools 
  UiTools 
  SerialPort 
  Designer)
find_package(QtHDF5 REQUIRED)
find_package(QMatPlotWidget REQUIRED)
find_package(Qt5PropertyBrowser REQUIRED)
find_package(QConsoleWidget REQUIRED)
find_package(muparser REQUIRED)

# Find modbus and GSL libraries using pkg-config
find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)

  pkg_check_modules(MODBUS REQUIRED IMPORTED_TARGET libmodbus)
  if(MODBUS_FOUND)
    message(STATUS "Modbus library found: ${MODBUS_LIBRARIES}")
  else()
    message(WARNING "Modbus library not found via pkg-config")
  endif()

  pkg_check_modules(GSL REQUIRED IMPORTED_TARGET gsl)
  if(GSL_FOUND)
    message(STATUS "GSL library found: ${GSL_LIBRARIES}")
  else()
    message(WARNING "GSL library not found via pkg-config")
  endif()

  if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    pkg_check_modules(GPIB IMPORTED_TARGET libgpib)
    if(GPIB_FOUND)
      message(STATUS "GPIB library found: ${GPIB_LIBRARIES}")
    else()
      message(WARNING "GPIB library not found via pkg-config")
    endif()
  endif()

else()
  message(WARNING "pkg-config not found, cannot search for Modbus, GSL or Linux-GPIB libraries")
endif()

if (WIN32)
  find_package(NI4882)
  if(NI4882_FOUND)
    message(STATUS "NI-488.2 library found: ${NI4882_LIBRARY}")
  else()
    message(WARNING "NI-488.2 library not found")
  endif()
endif()


set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Library, plugins & tools
add_subdirectory(src)
add_subdirectory(plugins)
add_subdirectory(tools)


message(STATUS "----------------------------------------")
message(STATUS "CMake configuration summary for ${PROJECT_NAME}")
message(STATUS "  CMake version:        ${CMAKE_VERSION} (Generator: ${CMAKE_GENERATOR})")
message(STATUS "  Project version:      ${PROJECT_VERSION}")
message(STATUS "  C++ standard:         ${CMAKE_CXX_STANDARD}")
message(STATUS "  Build type:           ${CMAKE_BUILD_TYPE}")
message(STATUS "  Install prefix:       ${CMAKE_INSTALL_PREFIX}")
message(STATUS "  CMAKE_PREFIX_PATH:    ${CMAKE_PREFIX_PATH}")
message(STATUS "  C++ compiler:         ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "  Qt version (found):   ${Qt5_VERSION}")
if(${HDF5_FOUND})
  message(STATUS "  HDF5 version:         ${HDF5_VERSION}")
else()
  message(STATUS "  HDF5:      <not found>")
endif()
if(muparser_FOUND)
  message(STATUS "  muparser version:     ${muparser_VERSION}")
else()
  message(STATUS "  muparser:   <not found>")
endif()

if(GSL_FOUND)
  message(STATUS "  GSL version:          ${GSL_VERSION}")
else()
  message(STATUS "  GSL:      <not found>")
endif()

if (WIN32)
  if(NI4882_FOUND)
    message(STATUS "  NI-488.2 library:     ${NI4882_LIBRARY}")
  else()
    message(STATUS "  NI-488.2: <not found>")
  endif()
else()
  if(GPIB_FOUND)
    message(STATUS "  GPIB version:          ${GPIB_VERSION}")
  else()
    message(STATUS "  GPIB:      <not found>")
  endif()
endif()

message(STATUS "----------------------------------------")

