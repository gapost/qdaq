add_executable(qdaq 
    main.cpp
    qdaq.qrc
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
install(TARGETS qdaq
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

