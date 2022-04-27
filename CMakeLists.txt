cmake_minimum_required(VERSION 3.21)

project(beet_engine)




add_subdirectory(third)

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

include_directories(beet
        core/inc/
        third/glfw/include/
        )

target_link_libraries(beet
        glfw
        spdlog)

set_target_properties(beet PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build")
set_target_properties(beet PROPERTIES OUTPUT_NAME beet_build)

#======== DEBUG ========#

if(CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_definitions(beet PUBLIC BEET_DEBUG=1)
    target_compile_definitions(beet PUBLIC _DEBUG=1)
else()
    target_compile_definitions(beet PUBLIC NDEBUG=1)
endif()
