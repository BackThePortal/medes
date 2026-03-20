include(FetchContent)
cmake_minimum_required(VERSION 4.0)

project(medes)
set(CMAKE_CXX_STANDARD 23)

option(MEDES_BUILD_EXAMPLES "Build example programs" ON)


FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz)
FetchContent_MakeAvailable(json)


set (CMAKE_CXX_FLAGS "-Wall")


# Ensure sub-projects can find the custom curlpp module
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/adapters/medes_curlpp/cmake")

add_subdirectory(core)

add_subdirectory(adapters/medes_curlpp)
add_subdirectory(adapters/medes_dpp)

if(MEDES_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

install(EXPORT medes-targets
        FILE medes-targets.cmake
        NAMESPACE medes::
        DESTINATION lib/cmake/medes
)

configure_package_config_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/medes-config.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/medes-config.cmake"
        INSTALL_DESTINATION lib/cmake/medes
)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/medes-config.cmake"
        DESTINATION lib/cmake/medes
)