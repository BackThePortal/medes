cmake_minimum_required(VERSION 4.0)
project(medes_core)
set(CMAKE_CXX_STANDARD 23)

add_library(medes_core STATIC
        src/string_build.cpp
        src/http_methods.cpp
)
target_include_directories(medes_core
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include> # This refers to /usr/local/include
)
target_compile_features(medes_core PUBLIC cxx_std_20)

install(TARGETS medes_core nlohmann_json
        EXPORT medes-targets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
)
install(
        DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
        DESTINATION include
        FILES_MATCHING
        PATTERN "*.h"
        PATTERN "version.h"
)
add_library(medes::core ALIAS medes_core)