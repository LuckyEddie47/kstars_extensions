cmake_minimum_required(VERSION 3.14)

project(firecapture_launcher LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core DBus)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core DBus)
add_executable(FC_launcher
  main.cpp

  log.h log.cpp
  kstarsinterface.h kstarsinterface.cpp
  kstarsDBusConf.h
  ekosStatus.h
  process.h process.cpp
  bombout.h

)
target_link_libraries(FC_launcher Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::DBus)

include(GNUInstallDirs)
install(TARGETS FC_launcher
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
