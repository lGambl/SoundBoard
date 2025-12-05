cmake_minimum_required(VERSION 3.16)

# Project name -> soundboard
project(soundboard VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Use a variable so the target name is consistent everywhere.
set(APP_TARGET soundboard)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Multimedia)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Multimedia)

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    resources.qrc
    model/item.h
    model/item.cpp
    model/viewmodel.h
    model/viewmodel.cpp
    model/itemdelegate.h
    model/itemdelegate.cpp
    model/filehandler.h
    model/filehandler.cpp
    model/audiomixer.h
    model/audiomixer.cpp
    model/sounditemwidget.cpp
    model/sounditemwidget.h
    model/keybinddialog.h
    model/keybinddialog.cpp
    model/hotkeymanager.h
    model/hotkeymanager.cpp
)

# Add local QHotkey subdirectory (ensure this path exists in your repo)
add_subdirectory(external/QHotkey)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(${APP_TARGET}
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
    # For Android with Qt6 you could set:
    # set_property(TARGET ${APP_TARGET} APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
    #              ${CMAKE_CURRENT_SOURCE_DIR}/android)
else()
    if(ANDROID)
        add_library(${APP_TARGET} SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(${APP_TARGET}
            ${PROJECT_SOURCES}
        )
    endif()
endif()

# Link libraries (Qt + QHotkey)
target_link_libraries(${APP_TARGET} PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Multimedia
    QHotkey
)

# QHotkey headers live in external/QHotkey/QHotkey
target_include_directories(${APP_TARGET} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/external/QHotkey/QHotkey
)

# Bundle / app metadata
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.soundboard)
endif()

set_target_properties(${APP_TARGET} PROPERTIES
    ${BUNDLE_ID_OPTION}
    OUTPUT_NAME soundboard                   # ensures binary name is 'soundboard'
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# Install
include(GNUInstallDirs)
install(TARGETS ${APP_TARGET}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Qt6 finalize
if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(${APP_TARGET})
endif()
