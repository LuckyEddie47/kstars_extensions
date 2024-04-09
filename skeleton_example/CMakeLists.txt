cmake_minimum_required(VERSION 3.14)

project(skeleton_example LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core DBus)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core DBus)
add_executable(example
  main.cpp

  log.h log.cpp
  kstarsinterface.h kstarsinterface.cpp
  kstarsDBusConf.h
  ekosStatus.h
  process.h process.cpp
  confchecker.cpp confchecker.h

)
target_link_libraries(example Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::DBus)

include(GNUInstallDirs)
install(TARGETS example
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

add_custom_target(copy-conf ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/example.conf)
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/example.conf
                   COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/example.conf
                                                    ${CMAKE_CURRENT_BINARY_DIR}/example.conf
                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/example.conf)

add_custom_target(copy-icon ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/example.png)
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/example.png
                   COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/example.png
                                                    ${CMAKE_CURRENT_BINARY_DIR}/example.png
                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/example.png)
