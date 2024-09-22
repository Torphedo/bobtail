#include <GLFW/glfw3.h>

#include <common/int.h>
#include "input.h"

input_internal input = {0};

void set_input_by_glfw_code(int key, bool state, int mods) {
    // Giant switch statement for every key...
    switch (key) {
        case GLFW_KEY_SPACE:
            input.space = state;
            break;
        case GLFW_KEY_APOSTROPHE:
            input.apostrophe = state;
            break;
        case GLFW_KEY_COMMA:
            input.comma = state;
            break;
        case GLFW_KEY_MINUS:
            input.minus = state;
            break;
        case GLFW_KEY_PERIOD:
            input.period = state;
            break;
        case GLFW_KEY_SLASH:
            input.slash = state;
            break;
        case GLFW_KEY_0:
            input.num_0 = state;
            break;
        case GLFW_KEY_1:
            input.num_1 = state;
            break;
        case GLFW_KEY_2:
            input.num_2 = state;
            break;
        case GLFW_KEY_3:
            input.num_3 = state;
            break;
        case GLFW_KEY_4:
            input.num_4 = state;
            break;
        case GLFW_KEY_5:
            input.num_5 = state;
            break;
        case GLFW_KEY_6:
            input.num_6 = state;
            break;
        case GLFW_KEY_7:
            input.num_7 = state;
            break;
        case GLFW_KEY_8:
            input.num_8 = state;
            break;
        case GLFW_KEY_9:
            input.num_9 = state;
            break;
        case GLFW_KEY_SEMICOLON:
            input.semicolon = state;
            break;
        case GLFW_KEY_EQUAL:
            input.equal = state;
            break;
        case GLFW_KEY_A:
            input.a = state;
            break;
        case GLFW_KEY_B:
            input.b = state;
            break;
        case GLFW_KEY_C:
            input.c = state;
            break;
        case GLFW_KEY_D:
            input.d = state;
            break;
        case GLFW_KEY_E:
            input.e = state;
            break;
        case GLFW_KEY_F:
            input.f = state;
            break;
        case GLFW_KEY_G:
            input.g = state;
            break;
        case GLFW_KEY_H:
            input.h = state;
            break;
        case GLFW_KEY_I:
            input.i = state;
            break;
        case GLFW_KEY_J:
            input.j = state;
            break;
        case GLFW_KEY_K:
            input.k = state;
            break;
        case GLFW_KEY_L:
            input.l = state;
            break;
        case GLFW_KEY_M:
            input.m = state;
            break;
        case GLFW_KEY_N:
            input.n = state;
            break;
        case GLFW_KEY_O:
            input.o = state;
            break;
        case GLFW_KEY_P:
            input.p = state;
            break;
        case GLFW_KEY_Q:
            input.q = state;
            break;
        case GLFW_KEY_R:
            input.r = state;
            break;
        case GLFW_KEY_S:
            input.s = state;
            break;
        case GLFW_KEY_T:
            input.t = state;
            break;
        case GLFW_KEY_U:
            input.u = state;
            break;
        case GLFW_KEY_V:
            input.v = state;
            break;
        case GLFW_KEY_W:
            input.w = state;
            break;
        case GLFW_KEY_X:
            input.x = state;
            break;
        case GLFW_KEY_Y:
            input.y = state;
            break;
        case GLFW_KEY_Z:
            input.z = state;
            break;
        case GLFW_KEY_LEFT_BRACKET:
            input.left_bracket = state;
            break;
        case GLFW_KEY_BACKSLASH:
            input.backslash = state;
            break;
        case GLFW_KEY_RIGHT_BRACKET:
            input.right_bracket = state;
            break;
        case GLFW_KEY_GRAVE_ACCENT:
            input.grave_accent = state;
            break;
        case GLFW_KEY_WORLD_1:
            input.world_1 = state;
            break;
        case GLFW_KEY_WORLD_2:
            input.world_2 = state;
            break;
    	case GLFW_KEY_ESCAPE:
            input.escape = state;
            break;
    	case GLFW_KEY_ENTER:
            input.enter = state;
            break;
    	case GLFW_KEY_TAB:
            input.tab = state;
            break;
    	case GLFW_KEY_BACKSPACE:
            input.backspace = state;
            break;
    	case GLFW_KEY_INSERT:
            input.insert = state;
            break;
    	case GLFW_KEY_DELETE:
            input.delete = state;
            break;
    	case GLFW_KEY_RIGHT:
            input.right = state;
            break;
    	case GLFW_KEY_LEFT:
            input.left = state;
            break;
    	case GLFW_KEY_DOWN:
            input.down = state;
            break;
    	case GLFW_KEY_UP:
            input.up = state;
            break;
    	case GLFW_KEY_PAGE_UP:
            input.page_up = state;
            break;
    	case GLFW_KEY_PAGE_DOWN:
            input.page_down = state;
            break;
    	case GLFW_KEY_HOME:
            input.home = state;
            break;
    	case GLFW_KEY_END:
            input.end = state;
            break;
    	case GLFW_KEY_CAPS_LOCK:
            input.caps_lock = state;
            break;
    	case GLFW_KEY_SCROLL_LOCK:
            input.scroll_lock = state;
            break;
    	case GLFW_KEY_NUM_LOCK:
            input.num_lock = state;
            break;
    	case GLFW_KEY_PRINT_SCREEN:
            input.print_screen = state;
            break;
    	case GLFW_KEY_PAUSE:
            input.pause = state;
            break;
    	case GLFW_KEY_F1:           
            input.f1 = state;
            break;
    	case GLFW_KEY_F2:           
            input.f2 = state;
            break;
    	case GLFW_KEY_F3:           
            input.f3 = state;
            break;
    	case GLFW_KEY_F4:           
            input.f4 = state;
            break;
    	case GLFW_KEY_F5:           
            input.f5 = state;
            break;
    	case GLFW_KEY_F6:           
            input.f6 = state;
            break;
    	case GLFW_KEY_F7:           
            input.f7 = state;
            break;
    	case GLFW_KEY_F8:           
            input.f8 = state;
            break;
    	case GLFW_KEY_F9:           
            input.f9 = state;
            break;
    	case GLFW_KEY_F10:          
            input.f10 = state;
            break;
    	case GLFW_KEY_F11:          
            input.f11 = state;
            break;
    	case GLFW_KEY_F12:          
            input.f12 = state;
            break;
    	case GLFW_KEY_F13:          
            input.f13 = state;
            break;
    	case GLFW_KEY_F14:          
            input.f14 = state;
            break;
    	case GLFW_KEY_F15:          
            input.f15 = state;
            break;
    	case GLFW_KEY_F16:          
            input.f16 = state;
            break;
    	case GLFW_KEY_F17:          
            input.f17 = state;
            break;
    	case GLFW_KEY_F18:          
            input.f18 = state;
            break;
    	case GLFW_KEY_F19:          
            input.f19 = state;
            break;
    	case GLFW_KEY_F20:          
            input.f20 = state;
            break;
    	case GLFW_KEY_F21:          
            input.f21 = state;
            break;
    	case GLFW_KEY_F22:          
            input.f22 = state;
            break;
    	case GLFW_KEY_F23:          
            input.f23 = state;
            break;
    	case GLFW_KEY_F24:          
            input.f24 = state;
            break;
    	case GLFW_KEY_F25:          
            input.f25 = state;
            break;
    }

    // Toggle the modifier keys if they're being held
    input.shift     = (!input.shift & (mods & GLFW_MOD_SHIFT));
    input.control   = (!input.control & (mods & GLFW_MOD_CONTROL));
    input.alt       = (!input.alt & (mods & GLFW_MOD_ALT));
    input.super     = (!input.super & (mods & GLFW_MOD_SUPER));
    input.caps_lock = (!input.caps_lock & (mods & GLFW_MOD_CAPS_LOCK));
    input.num_lock  = (!input.num_lock & (mods & GLFW_MOD_NUM_LOCK));
}

void input_update(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // RELEASE is 0, and both PRESS and REPEAT are > 0. So we can pass the
    // action code directly as a bool, and it will toggle the state correctly.
    set_input_by_glfw_code(key, action, mods);
}

void scroll_update(GLFWwindow* window, double x, double y) {
    input.scroll.x += x;
    input.scroll.y += y;
}

void cursor_update(GLFWwindow* window, double x, double y) {
    input.cursor.x = x;
    input.cursor.y = y;
}

void mouse_button_update(GLFWwindow* window, int button, int action, int mods) {
    input.click_left = action * (button == GLFW_MOUSE_BUTTON_LEFT);
    input.click_right = action * (button == GLFW_MOUSE_BUTTON_RIGHT);
    input.click_middle = action * (button == GLFW_MOUSE_BUTTON_MIDDLE);
    input.mouse_button_4 = action * (button == GLFW_MOUSE_BUTTON_4);
    input.mouse_button_5 = action * (button == GLFW_MOUSE_BUTTON_5);
}

void update_mods(GLFWwindow* window) {
    input.left_shift = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT);
    input.right_shift = glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT);
    input.shift = input.left_shift | input.right_shift;
    input.left_control = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL);
    input.right_control = glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL);
    input.control = input.left_control | input.right_control;
    input.alt = glfwGetKey(window, GLFW_KEY_LEFT_ALT) | glfwGetKey(window, GLFW_KEY_RIGHT_ALT);
}

void gamepad_update() {
    // Wipe previous gamepad state
    input.LS = (vec2s){0};
    input.RS = (vec2s){0};
    input.LT = 0;
    input.RT = 0;
    input.gp = (gamepad_t){0};

    for (u8 i = 0; i < GLFW_JOYSTICK_LAST; i++) {
        if (!glfwJoystickIsGamepad(i)) {
            // If it doesn't have gamepad mappings, it's not usable for us
            continue;
        }

        GLFWgamepadstate gamepad = {0};
        glfwGetGamepadState(i, &gamepad); // Get input

        // Sum up stick inputs from all available controllers
        input.LS.x += gamepad.axes[GLFW_GAMEPAD_AXIS_LEFT_X];
        input.LS.y += gamepad.axes[GLFW_GAMEPAD_AXIS_LEFT_Y];
        input.RS.x += gamepad.axes[GLFW_GAMEPAD_AXIS_RIGHT_X];
        input.RS.y += gamepad.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y];
        input.LT += gamepad.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER];
        input.RT += gamepad.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER];

        // Set a button "on" if any of the controllers have it pressed
        input.gp.a |= gamepad.buttons[GLFW_GAMEPAD_BUTTON_A];
        input.gp.b |= gamepad.buttons[GLFW_GAMEPAD_BUTTON_B];
        input.gp.x |= gamepad.buttons[GLFW_GAMEPAD_BUTTON_X];
        input.gp.y |= gamepad.buttons[GLFW_GAMEPAD_BUTTON_Y];
        input.gp.lb |= gamepad.buttons[GLFW_GAMEPAD_BUTTON_LEFT_BUMPER];
        input.gp.rb |= gamepad.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER];
        input.gp.l3 |= gamepad.buttons[GLFW_GAMEPAD_BUTTON_LEFT_THUMB];
        input.gp.r3 |= gamepad.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_THUMB];
        input.gp.select |= gamepad.buttons[GLFW_GAMEPAD_BUTTON_BACK];
        input.gp.start |= gamepad.buttons[GLFW_GAMEPAD_BUTTON_START];
        input.gp.up |= gamepad.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP];
        input.gp.down |= gamepad.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN];
        input.gp.left |= gamepad.buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT];
        input.gp.right |= gamepad.buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT];
    }

    // If for some reason someone is moving sticks on multiple controllers at
    // once, keep the results within range.
    input.LS.x = CLAMP(-1.0f, input.LS.x, 1.0f);
    input.LS.y = CLAMP(-1.0f, input.LS.y, 1.0f);
    input.RS.x = CLAMP(-1.0f, input.RS.x, 1.0f);
    input.RS.y = CLAMP(-1.0f, input.RS.y, 1.0f);
    input.LT = CLAMP(-1.0f, input.LT, 1.0f);
    input.RT = CLAMP(-1.0f, input.RT, 1.0f);
}

