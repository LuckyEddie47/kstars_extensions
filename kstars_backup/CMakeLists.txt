cmake_minimum_required(VERSION 3.5)

project(kstars_backup VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets DBus)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets DBus LinguistTools)

qt_standard_project_setup(I18N_TRANSLATED_LANGUAGES de fr)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp mainwindow.h mainwindow.ui
        archiver.cpp archiver.h
        confchecker.cpp confchecker.h
        kstarsinterface.cpp kstarsinterface.h
        kstarsVersion.h
        kstarsDBusConf.h
        ekosStatus.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(KS_backup
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        KS_backup.conf KS_backup.png README.md

    )
endif()

qt_add_translations(KS_backup
                    INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR}
                    TS_FILES kstars_backup_de.ts kstars_backup_fr.ts)

qt_add_lupdate(SOURCE_TARGETS KS_backup
               TS_FILES kstars_backup_de.ts kstars_backup_fr.ts
               PLURALS_TS_FILE kstars_backup_en.ts)

target_link_libraries(KS_backup PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
                                            Qt${QT_VERSION_MAJOR}::DBus)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.KS_backup)
endif()
set_target_properties(KS_backup PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS KS_backup
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(KS_backup)
endif()

add_custom_target(copy-conf ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/KS_backup.conf)
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/KS_backup.conf
                   COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/KS_backup.conf
                                                    ${CMAKE_CURRENT_BINARY_DIR}/KS_backup.conf
                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/KS_backup.conf)

add_custom_target(copy-icon ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/KS_backup.png)
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/KS_backup.png
                   COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/KS_backup.png
                                                    ${CMAKE_CURRENT_BINARY_DIR}/KS_backup.png
                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/KS_backup.png)
