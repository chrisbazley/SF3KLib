cmake_minimum_required(VERSION 3.10)
project(SF3KLib C)

include(FetchContent)

FetchContent_Declare(CBOS GIT_REPOSITORY https://github.com/chrisbazley/CBOSLib.git GIT_TAG main)

FetchContent_MakeAvailable(CBOS)

set(CMAKE_C_STANDARD 99)

if(MSVC)
    # MSVC equivalent of -Wall -Wextra is /W4
    add_compile_options(/W4)
else()
    # For GCC and Clang
    add_compile_options(-Wall -Wextra -Wsign-conversion -pedantic)
endif()

file(GLOB SOURCES "*.c")
file(GLOB PUBLIC_HEADERS "*.h")

add_library(SF3K STATIC ${SOURCES} ${PUBLIC_HEADERS})

set_target_properties(SF3K PROPERTIES
    PUBLIC_HEADER "${PUBLIC_HEADERS}"
    DEBUG_POSTFIX "dbg"
)

target_include_directories(SF3K PUBLIC .)

target_compile_definitions(SF3K PRIVATE
    $<$<CONFIG:Debug>:DEBUG_OUTPUT>
    $<$<CONFIG:Release>:NDEBUG>
)

target_compile_options(SF3K PRIVATE
    $<$<CONFIG:Release>:-O3>
)

# Header files depend on definitions from this dependency
target_link_libraries(SF3K PUBLIC
    CBOS
)

install(TARGETS SF3K
    ARCHIVE DESTINATION lib
    PUBLIC_HEADER DESTINATION include
)
