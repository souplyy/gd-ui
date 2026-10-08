cmake_minimum_required(VERSION 3.25)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
if(CMAKE_SYSTEM_NAME STREQUAL "iOS" OR IOS)
    set(CMAKE_OSX_ARCHITECTURES "arm64" CACHE STRING "Target architectures")
elseif(APPLE OR CMAKE_HOST_APPLE)
    set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "Target architectures")
endif()

project(GDUI VERSION 0.1.0 LANGUAGES CXX)

if(NOT DEFINED ENV{GEODE_SDK} OR NOT EXISTS "$ENV{GEODE_SDK}/CMakeLists.txt")
    message(FATAL_ERROR "Install Geode SDK 5.10.1 and set GEODE_SDK to its directory. See README.md.")
endif()

add_library(${PROJECT_NAME} SHARED
    src/main.cpp
    src/hooks/MenuLayer.cpp
    src/ui/TopBar.cpp
    src/ui/SvgIcon.cpp
)
target_include_directories(${PROJECT_NAME} PRIVATE src)
add_subdirectory("$ENV{GEODE_SDK}" "${CMAKE_CURRENT_BINARY_DIR}/geode")
setup_geode_mod(${PROJECT_NAME})
