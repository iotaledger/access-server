# fetch external libs
include(ExternalProject)

# fetch iota_common
include(FetchContent)
FetchContent_Declare(
        iota_common
        GIT_REPOSITORY http://github.com/iotaledger/iota_common.git
        GIT_TAG cf649803757abf48432d4fa60e9f27ac119bae5f
)

message(STATUS "Fetching iota_common")
FetchContent_MakeAvailable(iota_common)
