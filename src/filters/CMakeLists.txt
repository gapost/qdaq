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

set(TARGET_NAME QDaqFilters)

add_library(${TARGET_NAME} SHARED
    ${_SOURCES}
    ${_HEADERS}
    )

add_library(QDaq::Filters ALIAS ${TARGET_NAME})

if(WIN32)
    target_compile_definitions(${TARGET_NAME} PRIVATE QDAQ_LIBRARY)
endif()

target_link_libraries(${TARGET_NAME}
    PRIVATE
    Qt5::Core
    Qt5::Script
    QDaq::Core
    PkgConfig::GSL
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

