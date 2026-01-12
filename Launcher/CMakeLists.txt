cmake_minimum_required(VERSION 3.24)
project(GardenGateLauncher VERSION 0.1.0 LANGUAGES C CXX)

# ---- Target ----
add_executable(${PROJECT_NAME} WIN32)

# ---- Language / standard ----
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

# ---- Sources ----
file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/assets/*.rc"
)

file(GLOB IMGUI_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui/backends/imgui_impl_win32.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui/backends/imgui_impl_dx11.cpp"
)

set(XDELTA_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/xdelta/xdelta3/xdelta3.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/xdelta/xdelta3_wrapper.cpp"
)

target_sources(${PROJECT_NAME} PRIVATE
    ${PROJECT_SOURCES}
    ${IMGUI_SOURCES}
    ${XDELTA_SOURCES}
)

# ---- Includes ----
target_include_directories(${PROJECT_NAME} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/renderer"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/config"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/patcher"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ui"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/utils"
    "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty"
    "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui"
    "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui/backends"
    "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/xdelta"
    "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/xdelta/xdelta3"
    "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/nlohmann"
)

# ---- Definitions ----
target_compile_definitions(${PROJECT_NAME} PRIVATE
    WIN32
    _WINDOWS
    _CRT_SECURE_NO_WARNINGS
    XD3_WIN32=1
    XD3_POSIX=0
    NOT_MAIN=1
    EXTERNAL_COMPRESSION=0
    SECONDARY_DJW=1
    SECONDARY_FGK=1
    SIZEOF_SIZE_T=8
    SIZEOF_UNSIGNED_LONG_LONG=8
)

# ---- Source Specific Fixes ----
set_source_files_properties("${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/xdelta/xdelta3/xdelta3.c"
    PROPERTIES COMPILE_DEFINITIONS "XD3_WIN32=1;XD3_POSIX=0;WIN32=1"
)
set_source_files_properties("${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/xdelta/xdelta3/xdelta3.c" 
    PROPERTIES LANGUAGE C
)

# ---- Warnings / Compiler Options ----
target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W3 /Zc:preprocessor>
    $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra>
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra>
)

# ---- Libraries ----
if (MSVC)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        d3d11.lib
        dxgi.lib
        d3dcompiler.lib
        user32.lib
        gdi32.lib
        shell32.lib
        shlwapi.lib
        ole32.lib
        advapi32.lib
        comctl32.lib
    )
endif()

# ---- Output ----
set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME "GardenGate Launcher"
)