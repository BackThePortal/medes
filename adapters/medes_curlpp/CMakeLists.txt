cmake_minimum_required(VERSION 4.0)
project(medes_curlpp)



add_library(medes_curlpp STATIC
        src/curlpp_adapter.cpp
)

find_package(curlpp REQUIRED)

target_link_libraries(medes_curlpp
        PUBLIC curlpp::curlpp
        PUBLIC medes_core
        PRIVATE curl
)
target_include_directories(medes_curlpp PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)
install(TARGETS medes_curlpp
        DESTINATION lib
)

install(
        DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
        DESTINATION include/medes/adapters
        FILES_MATCHING
        PATTERN "*.h"
        PATTERN "version.h"
)
add_library(medes::curlpp ALIAS medes_curlpp)