set(target cryptography)

add_library(${target} sha512.c curve25519-donna.c aes.c sha256.c ref/ge25519.c ref/crypto_hash_sha512.c ref/fe25519.c ref/sc25519.c ref/keypair.c ref/open.c ref/sign.c ref/verify.c ref/utils.c)
target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/ref)
