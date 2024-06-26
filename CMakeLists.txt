cmake_minimum_required(VERSION 3.10)

project(bragii VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(taglib CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
    src/main.cpp
    src/modules/songs_manager/audio.cpp
    src/modules/songs_manager/isong.cpp
    src/modules/songs_manager/songs_manager.cpp
    src/service/cover_art_api.cpp
    src/service/spotdl.cpp
    src/service/ytdlp.cpp
    src/ui/download_song_dialog.cpp
    src/ui/download_song_dialog.ui
    src/ui/empty_song_dir_dialog.cpp
    src/ui/emptysongdirdialog.ui
    src/ui/mainwindow.cpp
    src/ui/mainwindow.ui
    src/ui/title_controller.cpp
    src/utils/filesys.cpp
    src/utils/network.cpp
    src/utils/string.cpp
    src/utils/system.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(bragii
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(bragii SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(bragii
            ${PROJECT_SOURCES}
        )
    endif()
endif()

set(LIBS dl pthread OpenSSL::SSL OpenSSL::Crypto TagLib::tag TagLib::tag_c TagLib::TagLib ZLIB::ZLIB)
target_link_libraries(bragii PRIVATE Qt${QT_VERSION_MAJOR}::Widgets ${LIBS})

set_target_properties(bragii PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS bragii
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
    qt_finalize_executable(bragii)
endif()
