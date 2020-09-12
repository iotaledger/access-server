include(ExternalProject)

# fetch access-sdk
include(FetchContent)
FetchContent_Declare(
        iota.c
        GIT_REPOSITORY https://github.com/iotaledger/iota.c.git
        GIT_TAG 7743c91a02a32001a1158bdf4cee3580ce973991
)

message(STATUS "Fetching iota.c")
FetchContent_MakeAvailable(iota.c)