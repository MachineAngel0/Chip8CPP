#ifndef INPUT_H
#define INPUT_H

#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "chip8.h"


// Keypad       Keyboard
// +-+-+-+-+    +-+-+-+-+
// |1|2|3|C|    |1|2|3|4|
// +-+-+-+-+    +-+-+-+-+
// |4|5|6|D|    |Q|W|E|R|
// +-+-+-+-+ => +-+-+-+-+
// |7|8|9|E|    |A|S|D|F|
// +-+-+-+-+    +-+-+-+-+
// |A|0|B|F|    |Z|X|C|V|
// +-+-+-+-+    +-+-+-+-+


inline bool e_key_pressed = false;
inline bool q_key_pressed = false;
inline bool up_key_pressed = false;
inline bool left_key_pressed = false;
inline bool down_key_pressed = false;
inline bool right_key_pressed = false;
inline bool space_key_pressed = false;


inline void key_callback(GLFWwindow* window, CHIP8* chip8)
{
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
    {
        chip8->keypad[0] = 1;
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
    {
        chip8->keypad[1] = 1;
    }
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
    {
        chip8->keypad[2] = 1;
    }
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
    {
        chip8->keypad[3] = 1;
    }

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        chip8->keypad[4] = 1;
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        chip8->keypad[5] = 1;
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        chip8->keypad[6] = 1;
    }
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
    {
        chip8->keypad[7] = 1;
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        chip8->keypad[8] = 1;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        chip8->keypad[9] = 1;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        chip8->keypad[10] = 1;
    }
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
    {
        chip8->keypad[11] = 1;
    }

    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
    {
        chip8->keypad[12] = 1;
    }
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
    {
        chip8->keypad[13] = 1;
    }
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
    {
        chip8->keypad[14] = 1;
    }
    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
    {
        chip8->keypad[15] = 1;
    }
}



#endif //INPUT_H
