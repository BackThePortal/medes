find_path(CURLPP_INCLUDE_DIR NAMES curlpp/*)

find_library(CURLPP_LIBRARIES NAMES curlpp "libcurlpp.a")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(curlpp DEFAULT_MSG CURLPP_LIBRARIES CURLPP_INCLUDE_DIR)

if(curlpp_FOUND AND NOT TARGET curlpp::curlpp)
    add_library(curlpp::curlpp UNKNOWN IMPORTED)
    set_target_properties(curlpp::curlpp PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${CURLPP_INCLUDE_DIR}"
            IMPORTED_LOCATION "${CURLPP_LIBRARIES}"
    )
endif()