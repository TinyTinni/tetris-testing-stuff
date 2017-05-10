#include "StdKeyboard.h"
#include "../Window.h"
#include <GLFW/glfw3.h>
#include <iostream>


void stdKeyLayout(MainWindow& window,int key, int /*scancode*/, int action,  int modifier)
{
    if (action != GLFW_RELEASE)
        return;
    switch(key)
    {
    case GLFW_KEY_ESCAPE:
        window.leaveMainLoop();
    case GLFW_KEY_ENTER:
        if (modifier == GLFW_MOD_ALT)
            window.fullscreen(!window.isFullscreen());
    default:
        break;
    };
}
