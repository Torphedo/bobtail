#ifndef GL_SETUP_H
#define GL_SETUP_H
/// @file gl_setup.h
/// @brief Utilities for setting up OpenGL & graphical window

#include <stdbool.h>

#include <GLFW/glfw3.h>
#include <common/int.h>

/// This enum only exists to make the callsite more readable with no LSP
typedef enum {
    DISABLE_DEBUG = false,
    ENABLE_DEBUG = true
}enable_debug_msg;

/// @brief Setup GLFW window and create an OpenGL 3.3 context on core profile.
/// @param window_name String to display as the window name
/// @param enable_debug If enabled and supported by the OpenGL driver, all
/// debug messages from OpenGL will be printed to the console
/// @param mouse_mode GLFW mouse input mode, just use 0 if you don't care
GLFWwindow* setup_opengl(s32 width, s32 height, const char* window_name, bool enable_debug, int mouse_mode, bool use_vsync);

/// @brief Toggles VSync
///
/// This is just syntax sugar wrapping glfwSwapInterval()
void set_vsync(bool interval);

#endif // GL_SETUP_H
