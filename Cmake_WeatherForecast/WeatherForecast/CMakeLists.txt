cmake_minimum_required(VERSION 3.16)

project(WeatherForecast VERSION 1.0 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Gui Widgets Network Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Widgets Network Sql)

set(PROJECT_SOURCES
    main.cpp
    widget.cpp widget.h widget.ui
    chartwidget.cpp chartwidget.h
    apiclient.cpp apiclient.h
    datacache.cpp datacache.h
    weatherdata.h
    weatherTool.h
    res.qrc
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/debug")


if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(WeatherForecast
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    add_executable(WeatherForecast
        ${PROJECT_SOURCES}
    )
endif()

target_link_libraries(WeatherForecast PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::Sql
)

set_target_properties(WeatherForecast PROPERTIES
    WIN32_EXECUTABLE TRUE
)

install(TARGETS WeatherForecast
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(WeatherForecast)
endif()
