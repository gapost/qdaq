set(_SOURCES 
    filtersplugin.cpp
    filters-plugin.json
)

set(_HEADERS  
    filtersplugin.h
)

set(TARGET_NAME QDaqFiltersPlugin)

add_library(${TARGET_NAME} SHARED
    ${_SOURCES}
    ${_HEADERS}
    )

set_target_properties(${TARGET_NAME} PROPERTIES 
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/script")

add_library(QDaq::FiltersPlugin ALIAS ${TARGET_NAME})

target_link_libraries(${TARGET_NAME}
    PRIVATE
    Qt5::Core
    Qt5::Script
    QDaq::Core
    QDaq::Filters
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
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/qt5/plugins/script/
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)