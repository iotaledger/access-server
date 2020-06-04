set(target asn)

# Set libCrypto (ssl) as default implementation
if(NOT DEFINED ${asn_impl})
set(asn_impl libCrypto)
endif()


# Internal module dependencies
set(dependencies json_parser cryptography)

# Depending on system libcrypto for ssl variant
if(asn_impl STREQUAL libCrypto)
set(external_dependencies -lcrypto)
endif()

# Module source files
set(source_files asn_auth.c asn_auth_helper.c implementations/${asn_impl}/asn_client.c implementations/${asn_impl}/asn_server.c implementations/${asn_impl}/asn_client.c implementations/${asn_impl}/asn_utils.c)

# Module include directories
set(include_dirs ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/implementations/${asn_impl})

add_library(${target} ${source_files})
target_include_directories(${target} PUBLIC ${include_dirs})
target_link_libraries(${target} PUBLIC ${external_dependencies} ${dependencies})
