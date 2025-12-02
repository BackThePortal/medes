include(FetchContent)
cmake_minimum_required(VERSION 4.0)
project(medes_core)
set(CMAKE_CXX_STANDARD 23)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz)
FetchContent_MakeAvailable(json)

add_library(medes_core STATIC
        src/method.cpp
        src/string_build.cpp
)

target_link_libraries(medes_core PUBLIC nlohmann_json::nlohmann_json)


target_include_directories(medes_core
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include> # This refers to /usr/local/include
)
target_compile_features(medes_core PUBLIC cxx_std_20)

install(TARGETS medes_core
        DESTINATION lib
)

install(
        DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
        DESTINATION include/medes
        FILES_MATCHING
        PATTERN "*.h"
        PATTERN "version.h"
)
add_library(medes::core ALIAS medes_core)