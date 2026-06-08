cmake_minimum_required(VERSION 3.10)
project(SF3KLib C)

include(FetchContent)

FetchContent_Declare(CBOS GIT_REPOSITORY https://github.com/chrisbazley/CBOSLib.git GIT_TAG main)

FetchContent_MakeAvailable(CBOS)

set(CMAKE_C_STANDARD 99)
set(CMAKE_XCODE_ATTRIBUTE_RUN_CLANG_STATIC_ANALYZER "YES")

if(MSVC)
    # MSVC equivalent of -Wall -Wextra is /W4
    add_compile_options(/W4 /wd4200 /wd4996 /wd4130)
else()
    # For GCC and Clang
    add_compile_options(-Wall -Wextra -Wsign-conversion -pedantic)
endif()

if(CMAKE_C_COMPILER_ID MATCHES "Clang|AppleClang")
    find_program(CLANG_TIDY_EXE
        NAMES clang-tidy
        HINTS "/opt/homebrew/opt/llvm/bin"
    )

    if(CLANG_TIDY_EXE)
        # 1. Base Clang-Tidy call configuration
        set(CLANG_TIDY_ARGS "${CLANG_TIDY_EXE};-checks=-*,clang-analyzer-*,-clang-analyzer-security.insecureAPI.*")

        # 2. On Apple machines, append the current platform SDK directory path
        if(APPLE)
            execute_process(COMMAND xcrun --show-sdk-path OUTPUT_VARIABLE MAC_SDK_PATH OUTPUT_STRIP_TRAILING_WHITESPACE)
            list(APPEND CLANG_TIDY_ARGS "--extra-arg=-isysroot" "--extra-arg=${MAC_SDK_PATH}")
        endif()

        # 3. Apply the modified list to CMake
        set(CMAKE_C_CLANG_TIDY "${CLANG_TIDY_ARGS}")
        message(STATUS "Clang-Tidy Static Analyzer enabled automatically.")
    else()
        message(STATUS "clang-tidy binary not found. Skipping analysis pass.")
    endif()
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
)

# Header files depend on definitions from this dependency
target_link_libraries(SF3K PUBLIC
    CBOS
)

install(TARGETS SF3K
    ARCHIVE DESTINATION lib
    PUBLIC_HEADER DESTINATION include
)
