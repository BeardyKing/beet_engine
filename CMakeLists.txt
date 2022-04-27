cmake_minimum_required(VERSION 3.21)

project(beet_engine)

include_directories(core/inc/)

file(GLOB_RECURSE BEET_CORE_SOURCES
        core/src/*.cpp
        core/inc/**/*.h
        )

file(GLOB_RECURSE BEET_EDITOR_SOURCES
        editor/**/*.cpp
        editor/**/*.h
        )

add_executable(beet
        ${BEET_CORE_SOURCES}
        ${BEET_EDITOR_SOURCES}
        )

set_target_properties(beet PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
        )

set_target_properties(beet PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build")
set_target_properties(beet PROPERTIES OUTPUT_NAME beet_build)


