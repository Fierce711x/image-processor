cmake_minimum_required(VERSION 3.20)

project(ImageProcessor LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

# Keep WebView's optional examples/tests/docs disabled.
set(WEBVIEW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(WEBVIEW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(WEBVIEW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(WEBVIEW_ENABLE_CHECKS OFF CACHE BOOL "" FORCE)
set(WEBVIEW_ENABLE_PACKAGING OFF CACHE BOOL "" FORCE)

# Let WebView's own CMake configure the WebView2 SDK on Windows.
set(WEBVIEW_USE_BUILTIN_MSWEBVIEW2 ON CACHE BOOL "" FORCE)
set(WEBVIEW_MSWEBVIEW2_VERSION "1.0.2903.40" CACHE STRING "" FORCE)

FetchContent_Declare(
    webview
    GIT_REPOSITORY https://github.com/webview/webview.git
    GIT_TAG master
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(webview)

# Your ui.cpp includes "webview.h". Generate a forwarding header so it can
# keep that include while using the WebView source version fetched above.
set(GENERATED_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
file(MAKE_DIRECTORY "${GENERATED_DIR}")
file(WRITE "${GENERATED_DIR}/webview.h"
    "#pragma once\n#include <webview/webview.h>\n"
)

# Download TinyFileDialogs; its source/header live in the repository's src folder.
FetchContent_Declare(
    tinyfiledialogs_source
    GIT_REPOSITORY https://github.com/polyfem/tinyfiledialogs.git
    GIT_TAG main
    GIT_SHALLOW TRUE
)
FetchContent_GetProperties(tinyfiledialogs_source)
if(NOT tinyfiledialogs_source_POPULATED)
    FetchContent_Populate(tinyfiledialogs_source)
endif()

if(NOT EXISTS "${tinyfiledialogs_source_SOURCE_DIR}/src/tinyfiledialogs.c")
    message(FATAL_ERROR
        "TinyFileDialogs was downloaded, but src/tinyfiledialogs.c was not found."
    )
endif()

add_executable(app
    ui.cpp
    "${tinyfiledialogs_source_SOURCE_DIR}/src/tinyfiledialogs.c"
)

target_include_directories(app PRIVATE
    "${GENERATED_DIR}"
    "${tinyfiledialogs_source_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

# WebView's official target supplies its platform-specific include paths and links.
target_link_libraries(app PRIVATE webview::core)

if(WIN32)
    # Native Windows file/color dialogs used by TinyFileDialogs.
    target_link_libraries(app PRIVATE
        advapi32
        comdlg32
        ole32
        oleaut32
        shell32
        shlwapi
        user32
        uuid
        version
    )
elseif(APPLE)
    # WebView's target supplies WebKit; dl is used by the WebView implementation.
    target_link_libraries(app PRIVATE dl)
else()
    message(FATAL_ERROR
        "This CMake file supports Windows and macOS. "
        "Linux additionally needs GTK and WebKitGTK development packages."
    )
endif()

message(STATUS "ImageProcessor configured; dependencies are managed by CMake.")
