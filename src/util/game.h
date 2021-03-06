//
// Created by sneeuwpop on 16-6-19.
//

#pragma once

#include <GLFW/glfw3.h>
#include "interfaces/screen.h"

namespace GAME
{
    void swapBuffers();
    bool init();
    void run();
    void setScreen(Screen *screen);
}