set(target pap)
add_library(${target} pap.c user/user.c validator/parser.c validator/validator.c )
target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/validator ${CMAKE_CURRENT_SOURCE_DIR}/user)
target_link_libraries(${target} PUBLIC json_parser cryptography config_manager)
