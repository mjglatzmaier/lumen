#include "input.h"
#include <stdio.h>
#include <string.h>

// Global input state
InputState inputState = {0};  // ✅ Define the global input state

// GLFW Callbacks
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key >= 0 && key < 512) {
        inputState.keys[key] = (action != GLFW_RELEASE);
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button >= 0 && button < 5) {
        inputState.mouseButtons[button] = (action != GLFW_RELEASE);
        
        if (action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_LEFT) {
            printf("🖱 Mouse Button Released\n");
            inputState.mouseReleased = true;  // ✅ Track mouse release globally
        }
    }
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    inputState.mouseX = (float)xpos;
    inputState.mouseY = (float)ypos;
}

// Input system initialization
void input_init(GLFWwindow* window) {
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
}

// Update input state (for future gamepad support)
void input_update() {
    // Future gamepad polling can go here
}

// Check if an action is pressed
bool input_is_action_pressed(InputAction action) {
    switch (action) {
        case INPUT_ACTION_MOVE_LEFT:
            return inputState.keys[GLFW_KEY_A] || inputState.keys[GLFW_KEY_LEFT];
        case INPUT_ACTION_MOVE_RIGHT:
            return inputState.keys[GLFW_KEY_D] || inputState.keys[GLFW_KEY_RIGHT];
        case INPUT_ACTION_JUMP:
            return inputState.keys[GLFW_KEY_SPACE];
        case INPUT_ACTION_SHOOT:
            return inputState.mouseButtons[GLFW_MOUSE_BUTTON_LEFT];
        default:
            return false;
    }
}

// Get mouse position
void input_get_mouse_position(float* x, float* y) {
    *x = inputState.mouseX;
    *y = inputState.mouseY;
}

// Cleanup (not needed for GLFW but useful for other APIs)
void input_cleanup() {
    memset(&inputState, 0, sizeof(InputState));
}
