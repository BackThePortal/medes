cmake_minimum_required(VERSION 4.0)
project(medes)

# Ensure sub-projects can find the custom curlpp module
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/adapters/medes_curlpp/cmake")

# Add the core library first
add_subdirectory(core)

# Add the adapter second (so it can see the medes_core target)
add_subdirectory(adapters/medes_curlpp)