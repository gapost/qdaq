set(_SOURCES 
    qledplugin.cpp 
    qdaqplotwidgetplugin.cpp 
    qdaqwidgetsplugin.cpp 
    qdaqconsoletabplugin.cpp
    icons.qrc
)

set(_HEADERS  
    qledplugin.h 
    qdaqplotwidgetplugin.h 
    qdaqwidgetsplugin.h 
    qdaqconsoletabplugin.h
)

set(TARGET_NAME QDaqDesignerPlugin)

add_library(${TARGET_NAME} SHARED
    ${_SOURCES}
    ${_HEADERS}
    )

set_target_properties(${TARGET_NAME} PROPERTIES 
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/designer")

add_library(QDaq::DesignerPlugin ALIAS ${TARGET_NAME})


target_link_libraries(${TARGET_NAME}
    PRIVATE
    Qt5::Core
    Qt5::Widgets
    Qt5::Designer
    QDaq::Core
    QDaq::Gui
    )

target_include_directories(${TARGET_NAME}
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/QDaq>
    )

# Install the library target and export targets file
if(WIN32)
    install(TARGETS ${TARGET_NAME}
        EXPORT ${TARGET_NAME}Targets
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/designer
    )
else()
    install(TARGETS ${TARGET_NAME}
        EXPORT ${TARGET_NAME}Targets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/qt5/plugins/designer/
    )
endif()
