# fetch external libs
include(ExternalProject)

# fetch pigpio
include(FetchContent)
FetchContent_Declare(
        pigpio
        GIT_REPOSITORY https://github.com/joan2937/pigpio.git
        GIT_TAG b6fe5874516eb67745fa6277d2060b715dac09a9
)

message(STATUS "Fetching pigpio")
FetchContent_MakeAvailable(pigpio)