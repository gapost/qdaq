set(_SOURCES 
    qdaqserial.cpp 
    qdaqmodbus.cpp 
    qdaqtcpip.cpp 
    tcp_socket.cpp 
    qdaqinterfaces.cpp
    linuxgpib.cpp
)

set(_HEADERS  
    qdaqserial.h 
    qdaqmodbus.h 
    qdaqtcpip.h 
    tcp_socket.h 
    qdaqinterfaces.h
    linuxgpib.h
)

set(TARGET_NAME QDaqInterfaces)

add_library(${TARGET_NAME} SHARED
    ${_SOURCES}
    ${_HEADERS}
    )

add_library(QDaq::Interfaces ALIAS ${TARGET_NAME})

target_compile_definitions(${TARGET_NAME} PRIVATE LINUX_GPIB)

target_link_libraries(${TARGET_NAME}
    PRIVATE
    Qt5::Core
    Qt5::Network
    Qt5::Script
    Qt5::SerialPort
    QDaq::Core
    PkgConfig::MODBUS
    PkgConfig::GPIB
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
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
