//
// Created by sneeuwpop on 23-6-19.
//

#include "keyboard.h"
#include "../debug/profiler.h"
#include <iostream>
#include <map>

// REMEMBER: we import this from  <GLFW/glfw3.h>
//
// #define GLFW_RELEASE                0
// #define GLFW_PRESS                  1
// #define GLFW_REPEAT                 2


namespace INPUT::KEYBOARD
{

    std::map<int, int> keyMap;
    bool firstKeyPressed = false;

    static void key_callback(GLFWwindow* window, int keyCode, int scancode, int action, int mods)
    {
        if (keyCode == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GLFW_TRUE);

        keyMap[keyCode] = action;
        firstKeyPressed = true;
    }

    void setup(GLFWwindow *window)
    {
        glfwSetKeyCallback(window, key_callback);
    }

    bool pressed(int keyCode) {
        return keyMap[keyCode] == GLFW_PRESS || keyMap[keyCode] == GLFW_REPEAT;
    }

    bool anyKeyEverPressed() {
        return firstKeyPressed;
    }

    void lateUpdate() {
        PROFILE_FUNCTION();
        // TODO: implement this
    }
}