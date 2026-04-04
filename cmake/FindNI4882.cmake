# - FindNI4882.cmake
#   Locate the NI-488.2 GPIB library and headers (ni4882.h and appropriate
#   library file for the platform).
#   The search is split into separate branches for Linux/Unix and Windows so
#   that callers on each platform can override hints or adapt behaviour
#   independently.
#   Sets the following variables on success:
#     NI4882_FOUND        - True if found
#     NI4882_INCLUDE_DIR  - Directory where ni4882.h was found
#     NI4882_LIBRARY      - Full path to the NI-488.2 library file
#     NI4882_LIBRARIES    - Same as NI4882_LIBRARY (for legacy support)
#     NI4882_INCLUDE_DIRS - Same as NI4882_INCLUDE_DIR (for compatibility)

if(WIN32)

  if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(_NI4882_ARCH_SUBDIR lib64)
    set(_NI4882_INCLUDE_FILE ni4882.h)
    set(_NI4882_OBJ_FILE ni4882.obj)
  else()
    set(_NI4882_ARCH_SUBDIR lib32)
    set(_NI4882_INCLUDE_FILE decl-32.h)
    set(_NI4882_OBJ_FILE gpib-32.obj)
  endif()

  # Windows search: library is typically a .lib file shipped with NI drivers
  find_path(NI4882_INCLUDE_DIR
            NAMES ${_NI4882_INCLUDE_FILE}
            PATHS
              $ENV{NIEXTCCOMPILERSUPP}/Include
              $ENV{NIEXTCCOMPILERSUPP}/include
              "${PROGRAMFILES}/National Instruments/Shared/ExternalCompilerSupport/C/include" # common install location
            NO_DEFAULT_PATH
          )

  

  find_file(NI4882_LIBRARY
               NAMES ${_NI4882_OBJ_FILE}
               PATHS
                 $ENV{NIEXTCCOMPILERSUPP}/${_NI4882_ARCH_SUBDIR}/msvc
                 "${PROGRAMFILES}/National Instruments/Shared/ExternalCompilerSupport/C/${_NI4882_ARCH_SUBDIR}/msvc" # fallback
               NO_DEFAULT_PATH
             )

  unset(_NI4882_ARCH_SUBDIR)
  unset(_NI4882_INCLUDE_FILE)
  unset(_NI4882_OBJ_FILE)

else()

  # Unix / Linux search: static library named libni4882.a
  find_path(NI4882_INCLUDE_DIR
            NAMES ni4882.h
            PATHS
              $ENV{NI4882_ROOT}/include
              /usr/include
              /usr/local/include
            NO_DEFAULT_PATH
          )

  find_library(NI4882_LIBRARY
               NAMES libni4882.a
               PATHS
                 $ENV{NI4882_ROOT}/lib
                 $ENV{NI4882_ROOT}/lib/${CMAKE_LIBRARY_ARCHITECTURE}
                 /usr/lib
                 /usr/lib/${CMAKE_LIBRARY_ARCHITECTURE}
                 /usr/lib64
                 /usr/local/lib
                 /usr/local/lib/${CMAKE_LIBRARY_ARCHITECTURE}
               NO_DEFAULT_PATH
             )
endif()

# Provide backward-compatible variables
set(NI4882_LIBRARIES "${NI4882_LIBRARY}" CACHE STRING "Libraries for NI-488.2")
set(NI4882_INCLUDE_DIRS "${NI4882_INCLUDE_DIR}" CACHE STRING "Include dirs for NI-488.2")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(NI4882
                                  REQUIRED_VARS NI4882_LIBRARY NI4882_INCLUDE_DIR
                                  VERSION_VAR NI4882_VERSION)

if(NI4882_FOUND)
  # convert to canonical names for consumers
  set(NI4882_FOUND TRUE CACHE BOOL "NI-488.2 library found" FORCE)
endif()

# allow advanced users to override hints and search behavior
mark_as_advanced(NI4882_INCLUDE_DIR NI4882_LIBRARY)
