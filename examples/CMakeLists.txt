function(medes_add_example name)
    cmake_parse_arguments(ARG "" "" "ADAPTERS" ${ARGN})

    add_executable(${name} ${ARG_UNPARSED_ARGUMENTS})

    target_link_libraries(${name}
            PRIVATE
            medes::core
            ${ARG_ADAPTERS}
    )

    set_target_properties(${name}
            PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples
            FOLDER examples
    )
endfunction()

medes_add_example(
        basic
        basic/main.cpp
        ADAPTERS medes::curlpp
)