set(target asn)

if(NOT DEFINED ${asn_impl})
set(asn_impl tinyembedded)
endif()

set(source_files asn_auth.c asn_auth_helper.c implementations/${asn_impl}/asn_client.c implementations/${asn_impl}/asn_server.c implementations/${asn_impl}/asn_client.c implementations/${asn_impl}/asn_utils.c)

add_library(${target} ${source_files})
target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/implementations/${asn_impl})
target_link_libraries(${target} PUBLIC json_parser cryptography)
