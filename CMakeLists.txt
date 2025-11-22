cmake_minimum_required(VERSION 3.10)
project(ned)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ========================
# Configuration & Settings
# ========================
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/util/config.h.in" "${CMAKE_BINARY_DIR}/config.h")
add_definitions(-DSOURCE_DIR="${CMAKE_SOURCE_DIR}")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(APPLE)
    add_compile_definitions(PLATFORM_MACOS)
    # Set minimum macOS version to 11.0 (Big Sur)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0" CACHE STRING "Minimum macOS version" FORCE)
    # Detect current architecture and target only that
    if(CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "arm64")
        set(CMAKE_OSX_ARCHITECTURES "arm64" CACHE STRING "Build architecture for macOS" FORCE)
    else()
        set(CMAKE_OSX_ARCHITECTURES "x86_64" CACHE STRING "Build architecture for macOS" FORCE)
    endif()
    # Add compatibility flags for macOS 11.0+
    add_compile_options(-mmacosx-version-min=11.0)
    add_link_options(-mmacosx-version-min=11.0)
elseif(WIN32)
    add_compile_definitions(PLATFORM_WINDOWS)
    add_compile_definitions(WIN32_LEAN_AND_MEAN)
    add_compile_definitions(NOMINMAX)
    add_compile_definitions(_WIN32_WINNT=0x0601) # Windows 7+
    # MSVC-specific flags from lsp-framework documentation
    if(MSVC)
        add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
        add_compile_definitions(UTF_CPP_CPLUSPLUS=202002L)
        add_compile_options(/std:c++20)
        # Only add essential C++20 recognition flag
        add_compile_options(/Zc:__cplusplus)
        # Enable parallel compilation for faster builds
        add_compile_options(/MP)
    endif()
elseif(UNIX)
    add_compile_definitions(PLATFORM_LINUX)
    # Ensure proper feature test macros for Linux to expose le16toh/be16toh functions
    add_compile_definitions(_GNU_SOURCE)
    add_compile_definitions(_DEFAULT_SOURCE)
endif()

find_package(CURL REQUIRED)
find_package(Freetype REQUIRED)
find_package(unofficial-libgit2 CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(utf8cpp CONFIG REQUIRED)
find_package(unofficial-tree-sitter CONFIG REQUIRED)
find_package(lsp CONFIG REQUIRED)

if(APPLE)
  add_definitions(-DGL_SILENCE_DEPRECATION)
  add_compile_options(-Wno-deprecated-declarations)
endif()

# Core library
# ================
# Project Sources
# ================
# Core application
set(APP_SOURCES
  main.cpp
  ned.cpp
  shaders/shader.cpp
  shaders/shader_types.cpp
  shaders/shader_manager.cpp
)

# Component groups
file(GLOB_RECURSE EDITOR_SOURCES    "editor/*.cpp")
file(GLOB_RECURSE AI_SOURCES        "ai/*.cpp")
file(GLOB_RECURSE AI_MM_SOURCES     "ai/*.mm")
file(GLOB_RECURSE FILES_SOURCES     "files/*.cpp")

set(LSP_SOURCES
  lsp/lsp_client.cpp
  lsp/lsp_symbol_info.cpp
  lsp/lsp_goto_def.cpp
  lsp/lsp_goto_ref.cpp
  lsp/lsp_uri_options.cpp
  lsp/lsp_dashboard.cpp
)

# Custom ImGui configuration to avoid modifying the submodule
#add_definitions(-DIMGUI_USER_CONFIG="ned_imgui_config.h")

# Utility library
add_library(ned_util STATIC
  util/settings.cpp
  util/settings_file_manager.cpp

)
target_include_directories(ned_util PUBLIC
  $<BUILD_INTERFACE:
    ${CMAKE_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/editor
    ${CMAKE_CURRENT_SOURCE_DIR}/files
  >
)
target_link_libraries(ned_util PUBLIC 
    unofficial::libgit2::libgit2
    nlohmann_json::nlohmann_json
    utf8::cpp utf8cpp::utf8cpp
)

add_library(ned_embed STATIC
  globals.cpp
  ned_embed.cpp
  ${EDITOR_SOURCES}
  ${LSP_SOURCES}
  ${AI_SOURCES}
  ${AI_MM_SOURCES}
  ${FILES_SOURCES}
  ${IMGUI_SOURCES}
)

target_include_directories(ned_embed PUBLIC
  $<BUILD_INTERFACE:
    ${CMAKE_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/editor
    ${CMAKE_CURRENT_SOURCE_DIR}/files
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/ai/mcp
  >
  $<TARGET_PROPERTY:Freetype::Freetype,INTERFACE_INCLUDE_DIRECTORIES>
)

target_link_libraries(ned_embed PUBLIC
    Freetype::Freetype
    CURL::libcurl 
    unofficial::libgit2::libgit2
    unofficial::tree-sitter::tree-sitter
    lsp::lsp
    ZLIB::ZLIB

    llhttp
    xdiff
    pcre

    $<$<PLATFORM_ID:Darwin>:iconv>
    $<$<PLATFORM_ID:Linux>:pthread>
    $<$<PLATFORM_ID:Linux>:ssl>
    $<$<PLATFORM_ID:Linux>:crypto>
    $<$<PLATFORM_ID:Linux>:pcre>
    $<$<PLATFORM_ID:Linux>:rt>
)
#[[
if(APPLE AND CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  target_link_libraries(ned PRIVATE
    "-framework Cocoa"
    "-framework CoreServices"
    "-framework QuartzCore" 
    "-framework IOKit"
    "-framework CoreVideo"
    "-framework OpenGL"
    "-framework Security"
    "-framework GSS"
    "-framework CoreFoundation"
  )
endif()

if(WIN32 AND CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  target_link_libraries(ned PRIVATE
    winhttp
    secur32
    crypt32
    rpcrt4
    ws2_32
    advapi32
    shell32
    ole32
    oleaut32
    user32
    comdlg32
  )
endif()
]]
# Add macOS frameworks for ned_embed when building on Apple platforms
if(APPLE)
  target_link_libraries(ned_embed PUBLIC
    "-framework Security"
    "-framework GSS"
    "-framework CoreFoundation"
  )
endif()

# ================
# Resources
# ================
file(COPY 
  ${CMAKE_CURRENT_SOURCE_DIR}/settings
  ${CMAKE_CURRENT_SOURCE_DIR}/fonts
  ${CMAKE_CURRENT_SOURCE_DIR}/icons
  ${CMAKE_CURRENT_SOURCE_DIR}/shaders
  ${CMAKE_CURRENT_SOURCE_DIR}/editor/queries 
  DESTINATION ${CMAKE_BINARY_DIR}
)

include(GNUInstallDirs)

install(
    TARGETS
        ned_embed
    EXPORT ned-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
install(
    DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.h"
)

install(EXPORT ned-targets
    FILE ned-targets.cmake
    NAMESPACE ned::
    DESTINATION share/ned
)
file(CONFIGURE
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/ned-config.cmake"
    CONTENT
        [=[
        include(CMakeFindDependencyMacro)

        find_dependency(slang CONFIG)

        include("${CMAKE_CURRENT_LIST_DIR}/ned-targets.cmake")
        ]=]
    @ONLY
)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/ned-config.cmake"
        DESTINATION share/ned
)
