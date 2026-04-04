set(_SOURCES
    qdaqserial.cpp
    qdaqmodbus.cpp
    qdaqtcpip.cpp
    tcp_socket.cpp
    qdaqinterfaces.cpp
)

set(_HEADERS
    qdaqserial.h
    qdaqmodbus.h
    qdaqtcpip.h
    tcp_socket.h
    qdaqinterfaces.h
)

if(GPIB_FOUND OR NI4882_FOUND)
    list(APPEND _SOURCES qdaqgpib.cpp)
    list(APPEND _HEADERS qdaqgpib.h)
endif()

set(TARGET_NAME QDaqInterfaces)

add_library(${TARGET_NAME} SHARED
    ${_SOURCES}
    ${_HEADERS}
    )

add_library(QDaq::Interfaces ALIAS ${TARGET_NAME})

if(GPIB_FOUND OR NI4882_FOUND)
    target_compile_definitions(${TARGET_NAME} PRIVATE QDAQ_GPIB)
endif()

if(WIN32)
    target_compile_definitions(${TARGET_NAME} PRIVATE QDAQ_LIBRARY)
    target_link_libraries(${TARGET_NAME}
        PRIVATE
        Qt5::Core
        Qt5::Network
        Qt5::Script
        Qt5::SerialPort
        QDaq::Core
        PkgConfig::MODBUS
        $<$<BOOL:${NI4882_FOUND}>:${NI4882_LIBRARY}>
        wsock32
        ws2_32
        )
    target_include_directories(${TARGET_NAME}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/QDaq>
        PRIVATE
        $<$<BOOL:${NI4882_FOUND}>:${NI4882_INCLUDE_DIR}>
        )
else()
    target_link_libraries(${TARGET_NAME}
        PRIVATE
        Qt5::Core
        Qt5::Network
        Qt5::Script
        Qt5::SerialPort
        QDaq::Core
        PkgConfig::MODBUS
        $<$<BOOL:${GPIB_FOUND}>:PkgConfig::GPIB>
        )
    target_include_directories(${TARGET_NAME}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/QDaq>
        )
endif()


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
