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
    3rdparty/jsedit/jsedit.cpp
    3rdparty/kled/kled.cpp
    qscriptcompleter.cpp
    qdaqgui.qrc
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
    3rdparty/jsedit/jsedit.h
    3rdparty/kled/kled.h
    qscriptcompleter.h
)

set(TARGET_NAME QDaqGui)

add_library(${TARGET_NAME} SHARED
    ${_SOURCES}
    ${_HEADERS}
    )

add_library(QDaq::Gui ALIAS ${TARGET_NAME} )

target_link_libraries(${TARGET_NAME} 
    PRIVATE
    Qt5::Core
    Qt5::Widgets
    Qt5::Script
    Qt5::ScriptTools
    Qt5::UiTools
    QtHDF5::QtHDF5
    QDaq::Core
    QMatPlotWidget::QMatPlotWidget
    QConsoleWidget::QConsoleWidget
    Qt5PropertyBrowser::Qt5PropertyBrowser
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


