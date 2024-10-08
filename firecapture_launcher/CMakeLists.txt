cmake_minimum_required(VERSION 3.14)

project(firecapture_launcher LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core DBus LinguistTools)

#Pre Qt6.7 translation style
set(TS_FILES
  FC_launcher_de.ts
  FC_launcher_fr.ts
  FC_launcher_es.ts)

QT_ADD_TRANSLATION(QM_FILES ${TS_FILES})
configure_file(firecapture_launcher.qrc ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)

add_executable(FC_launcher
  main.cpp
  log.h log.cpp
  kstarsinterface.h kstarsinterface.cpp
  kstarsDBusConf.h
  ekosStatus.h
  process.h process.cpp
  bombout.h
  FC_launcher.conf FC_launcher.png README.md
  ${CMAKE_CURRENT_BINARY_DIR}/firecapture_launcher.qrc
  ${QM_FILES}
)

target_link_libraries(FC_launcher Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::DBus)

include(GNUInstallDirs)
install(TARGETS FC_launcher
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

add_custom_target(copy-conf ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/FC_launcher.conf)
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/FC_launcher.conf
                   COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/FC_launcher.conf
                                                    ${CMAKE_CURRENT_BINARY_DIR}/FC_launcher.conf
                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/FC_launcher.conf)

add_custom_target(copy-icon ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/FC_launcher.png)
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/FC_launcher.png
                   COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/FC_launcher.png
                                                    ${CMAKE_CURRENT_BINARY_DIR}/FC_launcher.png
                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/FC_launcher.png)
