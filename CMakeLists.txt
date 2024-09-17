cmake_minimum_required(VERSION 3.16)
project(bobtail C)
set(CMAKE_C_STANDARD 11)
# Emit a compile_commands.json for use with clangd in LSPs
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

option(BOBTAIL_OPENGL "Also compile OpenGL helpers like shader compilation wrappers. Requires GLFW, GLAD, and CGLM")
option(BOBTAIL_TESTS "Build library unit tests")

set(extra_sources )
if (BOBTAIL_OPENGL)
    set(extra_sources
        common/gl/gl_debug.c
        common/gl/gl_setup.c
        common/gl/shader.c

        common/gl/input.c
        common/gl/vector.c
        common/gl/model.c
        common/gl/primitives.c
    )
endif()

add_library(common STATIC
    common/int.c
    common/file.c
    common/arguments.c
    common/logging.c
    common/sha1.c
    common/image.c
    common/path.c
    common/list.c
    common/queue.c
    common/vfile.c
    common/vmem.c
    ${extra_sources}
)
target_include_directories(common PUBLIC ${bobtail_SOURCE_DIR})

if (BOBTAIL_TESTS)
    add_executable(test
        ${test_sources}
        test/main.c
        test/test_list.c
        test/test_sha1.c
        test/test_vmem.c
    )
    target_include_directories(test PUBLIC ${bobtail_SOURCE_DIR})
    target_link_libraries(test PRIVATE common)
endif()
