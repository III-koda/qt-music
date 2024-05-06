cmake_minimum_required(VERSION 3.5)

project(qt-music-player VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
    src/main.cpp
    src/player/audio.cpp
    src/player/iplayer.cpp
    src/player/isong.cpp
    src/service/cover_art_api.cpp
    src/service/spotdl.cpp
    src/service/ytdlp.cpp
    src/ui/mainwindow.cpp
    src/ui/mainwindow.ui
    src/utils/filesys.cpp
    src/utils/string.cpp
    src/utils/system.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(qt-music-player
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(qt-music-player SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(qt-music-player
            ${PROJECT_SOURCES}
            src/ui/download_song_dialog.hpp src/ui/download_song_dialog.cpp src/ui/download_song_dialog.ui

        )
    endif()
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(LIBS dl pthread tag ssl crypto)
endif()
target_link_libraries(qt-music-player PRIVATE Qt${QT_VERSION_MAJOR}::Widgets ${LIBS})

set_target_properties(qt-music-player PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS qt-music-player
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

set(RESOURCES_DIR resources)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/${RESOURCES_DIR}"
        "${RESOURCES_DIR}"
    COMMENT "Copying ${RESOURCES_DIR} to build directory"
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(qt-music-player)
endif()
