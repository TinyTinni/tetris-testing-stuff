#include "Mouse.h"
#include <GLFW/glfw3.h>
#include <utility/Projection.h>

#include "../Data/QuadEntity.h"
#include "../Window.h"

void MouseControl::mouseButtonGlfwCallback(MainWindow& w, int button, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        if (button == GLFW_MOUSE_BUTTON_1)
        {
            double x, y;
            glfwGetCursorPos(w.getGLFWWindowPtr(), &x, &y);
            m_lastClicked[0] = x;
            m_lastClicked[1] = y;

            float2 p;
            p[0] = float(x);
            p[1] = float(y);
            m_quadID = m_controller.getQuadID(p);
           
            m_pressedTime = Timer::now();
        }
    }
    else if (action == GLFW_RELEASE)
    {
        if (button == GLFW_MOUSE_BUTTON_1)
        {
            m_lastClicked[0] = -1.f;
            m_lastClicked[1] = -1.f;
            m_quadID = 0;
            if (std::chrono::duration_cast<std::chrono::milliseconds>(Timer::now() - m_pressedTime).count() <= 100)
            {
                m_controller.rotateZ(1);
            }
        }
    }
}
void MouseControl::mousePosGlfwCallback(MainWindow& w, double x, double y)
{
    if (m_quadID != 0)
    {
        //todo project back
        float3 sidel;
        float3 origin;
        origin[0] = origin[1] = 0.f;
        sidel[0] = sidel[1] = Quad::sideLength();
        origin[2] = sidel[2] = 2.5f;
        
        float2 pos2d = globalProjection().project(sidel);
        float2 origin2d = globalProjection().project(origin);
        pos2d -= origin2d;
        while (m_lastClicked[0] - x >= pos2d[0])
        {
            m_controller.translateLeft();
            m_lastClicked[0] -= pos2d[0];
        }
        while (x - m_lastClicked[0] >= pos2d[0])
        {
            m_controller.translateRight();
            m_lastClicked[0] += pos2d[0];
        }
        while (y - m_lastClicked[1] >= pos2d[1])
        {
            m_controller.translateDown();
            m_lastClicked[1] += pos2d[1];
        }
    }
}