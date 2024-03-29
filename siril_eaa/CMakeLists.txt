cmake_minimum_required(VERSION 3.14)

project(Siril_EAA LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core DBus Network StateMachine)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core DBus Network StateMachine)

add_executable(Siril_EAA
  main.cpp

  log.h log.cpp
  kstarsinterface.h kstarsinterface.cpp
  kstarsDBusConf.h
  ekosStatus.h

  bombout.h
  statemachine.h statemachine.cpp
  confchecker.h confchecker.cpp
  kstarsVersion.h
  sirilinterface.h sirilinterface.cpp

)
target_link_libraries(Siril_EAA Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::DBus
    Qt${QT_VERSION_MAJOR}::Network Qt${QT_VERSION_MAJOR}::StateMachine)

include(GNUInstallDirs)
install(TARGETS Siril_EAA
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
