cmake_minimum_required(VERSION 3.14)

project(Launcher LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core DBus LinguistTools)

#Pre Qt6.7 translation style
set(TS_FILES
  launcher_de.ts
  launcher_fr.ts
  launcher_es.ts)

QT_ADD_TRANSLATION(QM_FILES ${TS_FILES})
configure_file(launcher.qrc ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)

add_executable(Launcher
  main.cpp
  log.h log.cpp
  kstarsinterface.h kstarsinterface.cpp
  kstarsDBusConf.h
  ekosStatus.h
  process.h process.cpp
  bombout.h
  Launcher.conf Launcher.png README.md
  ${CMAKE_CURRENT_BINARY_DIR}/launcher.qrc
  ${QM_FILES}
)

target_link_libraries(Launcher Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::DBus)

include(GNUInstallDirs)
install(TARGETS Launcher
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

add_custom_target(copy-conf ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/Launcher.conf)
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/Launcher.conf
                   COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Launcher.conf
                                                    ${CMAKE_CURRENT_BINARY_DIR}/Launcher.conf
                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/Launcher.conf)

add_custom_target(copy-icon ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/Launcher.png)
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/Launcher.png
                   COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Launcher.png
                                                    ${CMAKE_CURRENT_BINARY_DIR}/Launcher.png
                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/Launcher.png)
