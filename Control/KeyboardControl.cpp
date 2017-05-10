#include "KeyboardControl.h"
#include <GLFW/glfw3.h>

#include "../Data/SimpleTetris.h"

void KeyboardControl::keyboardGlfwCallback(MainWindow& /*mw*/, int key, int scancode, int action,  int modifier)
{
    if (action != GLFW_RELEASE)
        return;
    switch(key)
    {
    case GLFW_KEY_RIGHT:
        m_controler.translateRight();
        break;
    case GLFW_KEY_LEFT:
        m_controler.translateLeft();
        break;
    case GLFW_KEY_DOWN:
        m_controler.translateDown();
        break;
    case GLFW_KEY_SPACE:
        m_controler.dump();
        break;
    case GLFW_KEY_R:        
        m_controler.rotateZ(1);
        break;
    default:
        break;
    };
}

