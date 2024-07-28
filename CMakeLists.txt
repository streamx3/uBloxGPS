cmake_minimum_required(VERSION 3.16)

project(uBloxGPS LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core SerialPort)

add_executable(uBloxGPS
    uBloxGPS.h
    uBloxGPS.cpp
    main.cpp
)

# target_link_libraries(uBloxGPS
#     Qt${QT_VERSION_MAJOR}::Core
# )

target_link_libraries(uBloxGPS PRIVATE
    Qt::Core
    Qt::SerialPort
)

include(GNUInstallDirs)
install(TARGETS uBloxGPS
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
