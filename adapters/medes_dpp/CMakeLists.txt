cmake_minimum_required(VERSION 4.0)
project(medes_dpp)

add_library(medes_dpp STATIC
        src/dpp_adapter.cpp
)

find_package(dpp REQUIRED)

target_link_libraries(medes_dpp
        PUBLIC medes_core
        PUBLIC dpp::dpp
)

set_target_properties(medes_dpp
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/adapters
        FOLDER adapters
)

target_include_directories(medes_dpp PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)
install(TARGETS medes_dpp
        EXPORT medes-targets
        DESTINATION lib
)

install(
        DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
        DESTINATION include
        FILES_MATCHING
        PATTERN "*.h"
)
add_library(medes::medes_dpp ALIAS medes_dpp)