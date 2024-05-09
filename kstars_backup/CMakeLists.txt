cmake_minimum_required(VERSION 3.5)

project(kstars_backup VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets DBus LinguistTools)

#Pre Qt6.7 translation style
set(TS_FILES
  KS_backup_de.ts
  KS_backup_fr.ts
  KS_backup_es.ts)

QT5_ADD_TRANSLATION(QM_FILES ${TS_FILES})
#qt_create_translation(QM_FILES ${TS_FILES})
configure_file(KS_backup.qrc ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp mainwindow.h mainwindow.ui
        archiver.cpp archiver.h
        confchecker.cpp confchecker.h
        kstarsinterface.cpp kstarsinterface.h
        kstarsVersion.h
        kstarsDBusConf.h
        ekosStatus.h
        ${QM_FILES}
)

 add_executable(KS_backup
        ${PROJECT_SOURCES}
        KS_backup.conf KS_backup.png README.md
        ${QM_FILES}        
        ${CMAKE_CURRENT_BINARY_DIR}/KS_backup.qrc
    )

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
