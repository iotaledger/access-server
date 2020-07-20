cmake_minimum_required(VERSION 3.11)

cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})

project(access_wallet DESCRIPTION "Access Wallet Module")

enable_language(C)
enable_testing()

# libs in the sandbox
link_directories("${CMAKE_INSTALL_PREFIX}/lib")

# your source files
add_library(access_wallet "wallet.c" "wallet_errors.c")

target_link_directories(access_wallet PUBLIC
  "${CMAKE_INSTALL_PREFIX}/lib"
  "${iota.c_BUILD_DIR}")

target_include_directories(access_wallet PUBLIC
  "${PROJECT_SOURCE_DIR}"
  "${CMAKE_INSTALL_PREFIX}/include"
  "${CMAKE_INSTALL_PREFIX}/include/cjson"
  "${iota.c_SOURCE_DIR}"
)

set(wallet_libs
  cclient
  cjson
  http_parser
  common
  logger
  keccak
  mbedtls
  mbedcrypto
  mbedx509
  m
)

target_link_libraries(access_wallet PUBLIC ${wallet_libs})

if(TEST_WALLET)

  add_executable(test_wallet "tests/test_wallet.c")

  target_include_directories(test_wallet PRIVATE
  "${PROJECT_SOURCE_DIR}"
  "${CMAKE_INSTALL_PREFIX}/include"
  "${CMAKE_INSTALL_PREFIX}/include/cjson"
  "${iota.c_SOURCE_DIR}"
  )

  add_dependencies(test_wallet access_wallet)
  target_link_libraries(test_wallet PRIVATE
    unity
    access_wallet
    ${wallet_libs}
  )
  add_test(test_wallet test_wallet)

endif(TEST_WALLET)
