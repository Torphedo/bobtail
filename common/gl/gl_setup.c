#include <stdbool.h>
#include <stdio.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "logging.h"
#include "int.h"
#include "input.h"
#include "gl_debug.h"

void frame_resize_callback(GLFWwindow* window, int width, int height) {
    int framebuf_width = 0;
    int framebuf_height = 0;
    glfwGetFramebufferSize(window, &framebuf_width, &framebuf_height);
    glViewport(0, 0, framebuf_width, framebuf_height);
}

void glfw_error(int err_code, const char* msg) {
    // Saying "GLFW error" in the message is redundant because this function
    // name is printed as part of LOG_MSG().
    LOG_MSG(error, "[code %d] %s\n", err_code, msg);
}

void set_vsync(bool interval) {
    glfwSwapInterval(interval);
}

GLFWwindow* setup_opengl(s32 width, s32 height, const char* window_name, bool enable_debug, int mouse_mode, bool use_vsync) {
    glfwSetErrorCallback(glfw_error);

    // Setup GLFW
    if (!glfwInit()) {
        LOG_MSG(error, "GLFW init failure!\n");
        return NULL;
    }

    // Use OpenGL Core 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // Request a debug context if requested by caller
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, enable_debug);
    glfwWindowHint(GLFW_SAMPLES, 4); // 4-sample MSAA
    
    GLFWwindow* window = glfwCreateWindow(width, height, window_name, NULL, NULL);
    if (window == NULL) {
        LOG_MSG(error, "failed to create GLFW window of size %dx%d.\n", width, height);
        glfwTerminate();
        return NULL;
    }

    // Start tracking input state & using virtual cursor positions.
    glfwSetKeyCallback(window, input_update);
    glfwSetCursorPosCallback(window, cursor_update);
    glfwSetScrollCallback(window, scroll_update);
    glfwSetMouseButtonCallback(window, mouse_button_update);

    if (mouse_mode == 0) {
        mouse_mode = GLFW_CURSOR_DISABLED;
    }

    glfwSetInputMode(window, GLFW_CURSOR, mouse_mode);

    // Get non-accelerated input if possible
    LOG_MSG(info, "Raw mouse motion ");
    if (mouse_mode == GLFW_CURSOR_DISABLED && glfwRawMouseMotionSupported()) {
        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
        printf("enabled.\n");
    }
    else {
        printf("disabled.\n");
    }
    
    // Create the OpenGL context
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        LOG_MSG(error, "failed to initialize GLAD for OpenGL Core 3.3\n");
        glfwTerminate();
        return NULL;
    }
    
    if (enable_debug) {
        gl_debug_setup();
        // Example code that will trigger a critical debug message:
        // glBindBuffer(GL_VERTEX_ARRAY_BINDING, 0);
    }
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE); // Enable MSAA
    glEnable(GL_CULL_FACE); // Backface culling via winding order

    // Enable transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);


    // Set OpenGL viewport to size of window, handle resizing
    glViewport(0, 0, width, height);
    glfwSetFramebufferSizeCallback(window, frame_resize_callback);
    set_vsync(use_vsync);

    return window;
}

