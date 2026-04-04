if(WIN32)
    set(_WIN32_SOURCES qdaq.rc)
endif()

add_executable(qdaq  # WIN32 - add this options to avoid console window on Windows
    main.cpp
    qdaq.qrc
    ${_WIN32_SOURCES}
    )

target_link_libraries(qdaq
    PRIVATE
    Qt5::Core 
    Qt5::Widgets 
    Qt5::Script 
    QDaq::Core 
    QDaq::Gui 
    )

# Install the library target and export targets file
if(WIN32)
    install(TARGETS qdaq
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}
    )
else()
    install(TARGETS qdaq
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
endif()

