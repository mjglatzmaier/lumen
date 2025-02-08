#ifndef INPUT_H
#define INPUT_H

#include <GLFW/glfw3.h>
#include <stdbool.h>

// Enum for supported input devices
typedef enum {
    INPUT_DEVICE_KEYBOARD,
    INPUT_DEVICE_MOUSE,
    INPUT_DEVICE_GAMEPAD
} InputDevice;

// Enum for common input actions (can be expanded)
typedef enum {
    INPUT_ACTION_MOVE_LEFT,
    INPUT_ACTION_MOVE_RIGHT,
    INPUT_ACTION_JUMP,
    INPUT_ACTION_SHOOT
} InputAction;

// Struct to hold input state
typedef struct {
    bool keys[512];       // Keyboard state
    bool mouseButtons[5]; // Mouse button state
    float mouseX, mouseY; // Mouse position
    float gamepadAxes[8]; // Gamepad analog sticks
    bool mouseReleased;
} InputState;

extern InputState inputState;

// Function prototypes
void input_init(GLFWwindow* window);
void input_update();
bool input_is_action_pressed(InputAction action);
void input_get_mouse_position(float* x, float* y);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void input_cleanup();

#endif
