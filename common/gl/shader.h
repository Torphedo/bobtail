#ifndef SHADER_H
#define SHADER_H
#include <stdbool.h>
/// @file shader.h
/// @brief Utility functions to quickly compile shaders
///
/// Shader code is specific to each GPU, so we have to store the GLSL source
/// code as a string and compile it at runtime. Loading the shaders from a file
/// breaks if the files get moved/deleted, so it's better to store them inside
/// the program itself.

#include <glad/glad.h>
#include <common/int.h>

/// @brief Ensure the shader program linked correctly
/// @param shader OpenGL shader resource ID
/// @note This can allocate CPU memory, in the rare case that the shader
/// compiler log is extremely long (>8KiB).
bool shader_link_check(gl_obj shader);

/// @brief Compile a shader from GLSL source in-memory
/// @note This creates OpenGL resources that need to be freed later.
///
/// @param src GLSL shader source code
/// @param shader_type GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, or GL_GEOMETRY_SHADER
/// @return shader id for OpenGL, or 0 on failure
gl_obj shader_compile_src(const char* src, GLenum shader_type);

/// @brief A wrapper to compile & link a whole shader program at once.
///
/// You should call shader_link_check() afterwards in case of failure.
/// @note This creates OpenGL resources that need to be freed later.
/// @param vert_src GLSL source code for the vertex shader
/// @param frag_src GLSL source code for the fragment shader
/// @return shader program id for OpenGL, or 0 on failure
gl_obj program_compile_src(const char* vert_src, const char* frag_src);

#endif // #ifndef SHADER_H
