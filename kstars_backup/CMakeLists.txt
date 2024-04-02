cmake_minimum_required(VERSION 3.5)

project(kstars_backup VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets DBus)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets DBus)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(kstars_backup
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        archiver.cpp archiver.h confchecker.cpp confchecker.h kstarsVersion.h
        kstarsDBusConf.h kstarsinterface.cpp kstarsinterface.h
        ekosStatus.h
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET kstars_backup APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(kstars_backup SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(kstars_backup
            ${PROJECT_SOURCES}
            archiver.h archiver.cpp
            confchecker.cpp confchecker.h
            kstarsVersion.h
        )
    endif()
endif()

target_link_libraries(kstars_backup PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
                                            Qt${QT_VERSION_MAJOR}::DBus)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.kstars_backup)
endif()
set_target_properties(kstars_backup PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS kstars_backup
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(kstars_backup)
endif()

add_custom_target(copy-test-makefile ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/Backup.conf)
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/Backup.conf
                   COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Backup.conf
                                                    ${CMAKE_CURRENT_BINARY_DIR}/Backup.conf
                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/Backup.conf)
