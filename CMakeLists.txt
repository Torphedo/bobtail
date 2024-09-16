cmake_minimum_required(VERSION 3.16)
project(common C)
set(CMAKE_C_STANDARD 11)

option(COMMON_OPENGL "Also compile OpenGL helpers like shader compilation wrappers. Requires GLFW, GLAD, and CGLM")

set(extra_sources )
if (COMMON_OPENGL)
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

add_executable(test
    ${test_sources}
    test/main.c
    test/test_list.c
)
target_include_directories(test PUBLIC ${CMAKE_SOURCE_DIR})
target_link_libraries(test PRIVATE common)
