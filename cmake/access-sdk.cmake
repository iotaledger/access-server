include(ExternalProject)

# fetch access-sdk
include(FetchContent)
FetchContent_Declare(
        access-sdk
        GIT_REPOSITORY ssh://git@github.com/iotaledger/access-sdk.git
        GIT_TAG ab375a4e986101c505d5aed8207a5676f75f47b1)

message(STATUS "Fetching access-sdk")
FetchContent_MakeAvailable(access-sdk)
