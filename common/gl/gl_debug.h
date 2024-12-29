#ifndef GL_DEBUG_H
#define GL_DEBUG_H
#ifdef __cplusplus
extern "C" {
#endif
/// @file gl_debug.h
/// @brief OpenGL debug message handler

/// Enable debug output, and print all messages coming from OpenGL from now on.
void gl_debug_setup();

#ifdef __cplusplus
}
#endif
#endif // GL_DEBUG_H