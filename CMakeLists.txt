cmake_minimum_required(VERSION 3.21)

project(beet_engine)
add_subdirectory(third)

#======== SOURCE ==========#

file(GLOB_RECURSE BEET_CORE_SOURCES
        core/src/*.cpp
        core/inc/**/*.h
        )

file(GLOB_RECURSE BEET_EDITOR_SOURCES
        editor/**/*.cpp
        editor/**/*.h
        )

#==========================#

add_executable(beet
        ${BEET_CORE_SOURCES}
        ${BEET_EDITOR_SOURCES}
        )

include_directories(beet
        core/inc/
        third/glad/include/
        third/glfw/include/
        third/glm/
        third/assimp/include
        third/stb/
        third/entt/single_include
        )

target_link_libraries(beet
        glad
        glfw
        spdlog
        glm
        assimp
        stb
        EnTT::EnTT
        )

#======= PROPERTIES ========#

set_target_properties(beet PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
set_target_properties(beet PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/dist/bin")
set_target_properties(beet PROPERTIES OUTPUT_NAME beet_build)

#==========================#

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/dist)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/dist/bin)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/dist/res)

#======= RESOURCES ========#

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/dist/res)

file(GLOB_RECURSE BEET_RESOURCES LIST_DIRECTORIES false "${CMAKE_SOURCE_DIR}/res/*")
add_custom_target(beet_resources ALL DEPENDS ${BEET_RESOURCES})

foreach (RESOURCE_FILE ${BEET_RESOURCES})
    get_filename_component(FILE_NAME ${RESOURCE_FILE} NAME)
    get_filename_component(PARENT_DIR ${RESOURCE_FILE} DIRECTORY)
    string(REGEX REPLACE "^${CMAKE_SOURCE_DIR}/res" "" PARENT_DIR ${PARENT_DIR})

    set(FILE_NAME "${CMAKE_BINARY_DIR}/dist/res/${PARENT_DIR}/${FILE_NAME}")
    configure_file(${RESOURCE_FILE} ${FILE_NAME} COPYONLY)
endforeach ()

#==========================#

#========= DEBUG ==========#


if (CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_definitions(beet PUBLIC BEET_DEBUG=1)
    target_compile_definitions(beet PUBLIC _DEBUG=1)
else ()
    target_compile_definitions(beet PUBLIC NDEBUG=1)
endif ()

#==========================#
