# fetch external libs
include(ExternalProject)

# fetch sqlite-amalgamation
include(FetchContent)
FetchContent_Declare(
        sqlite3
        GIT_REPOSITORY https://github.com/azadkuh/sqlite-amalgamation.git
        GIT_TAG 9be05e13408dd8cf8dafe8f11b1700a28194e07e
)

message(STATUS "Fetching sqlite3")
FetchContent_MakeAvailable(sqlite3)
