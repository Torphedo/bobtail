cmake_minimum_required(VERSION 3.13)
project(bobtail C)
set(CMAKE_C_STANDARD 11)
# Emit a compile_commands.json for use with clangd in LSPs
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

option(BOBTAIL_OPENGL "Include OpenGL helpers like shader compilation wrappers. Requires GLFW, GLAD, and CGLM")
option(BOBTAIL_TESTS "Build library unit tests")

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

add_library(bobtail STATIC
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

    # Files for other platforms will just be empty and compile instantly
    common/vmem_posix.c
    common/vmem_windows.c
    common/vmem_switch.c
    ${extra_sources}
)

target_include_directories(bobtail PUBLIC ${bobtail_SOURCE_DIR})

# I want to add a "bobtail::" namespace, but for some reason CMake only allows
# you to declare a target with a normal name, *then* alias it to have a
# namespace. I'm not sure what the namespacing acheives, because the old
# (potentially conflicting generic name like "test") is still visible if used
# via add_subdirectory(). So we're just not doing namespaces for now, in spite
# of what appears to be the accepted best practice.
if (BOBTAIL_TESTS)
    add_executable(bobtail_test
        ${test_sources}
        test/main.c
        test/test_list.c
        test/test_queue.c
        test/test_sha1.c
        test/test_vmem.c
    )
    target_include_directories(bobtail_test PUBLIC ${bobtail_SOURCE_DIR})
    target_link_libraries(bobtail_test PRIVATE bobtail)
endif()
