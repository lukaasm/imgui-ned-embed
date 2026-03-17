cmake_minimum_required(VERSION 3.10)
project(imgui-code-editor)

if(MSVC)
  # Match vcpkg x64-windows-static-md triplet (/MD, /MDd) to avoid
  # unresolved STL runtime symbols when linking third-party static libs.
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
endif()

# ========================
# Configuration & Settings
# ========================
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/util/config.h.in" "${CMAKE_BINARY_DIR}/config.h")
add_definitions(-DSOURCE_DIR="${CMAKE_SOURCE_DIR}")

find_package(CURL REQUIRED)
find_package(libgit2 CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(utf8cpp CONFIG REQUIRED)
find_package(unofficial-tree-sitter CONFIG REQUIRED)
find_package(lsp CONFIG REQUIRED)


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
    libgit2::libgit2package
    nlohmann_json::nlohmann_json
    utf8::cpp utf8cpp::utf8cpp
)

add_library(ned_embed STATIC
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
)

target_link_libraries(ned_embed PUBLIC
    libgit2::libgit2package
    unofficial::tree-sitter::tree-sitter
    lsp::lsp
    CURL::libcurl
    ZLIB::ZLIB
)

find_package(hello-imgui CONFIG REQUIRED)

add_executable(editor WIN32 main.cpp)
target_link_libraries( editor PUBLIC
  hello-imgui::hello_imgui
  ned_embed
)

