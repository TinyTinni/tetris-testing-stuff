#pragma once

#include <chrono>
#include "Controller.h"
#include <utility/Vector.hpp>

class MouseControl
{
    typedef std::chrono::high_resolution_clock Timer;
    Controller& m_controller;
    bool m_leftPressed;
    double2 m_lastClicked;
    size_t m_quadID;
    std::chrono::time_point<Timer> m_pressedTime;
public:
    MouseControl(Controller& cont) :m_controller(cont)
    {
        m_lastClicked[0] = -1.f;
        m_lastClicked[1] = -1.f;
        m_leftPressed = false;
        m_quadID = 0;
    }

    void mouseButtonGlfwCallback(MainWindow& w, int button, int action, int mods);
    void mousePosGlfwCallback(MainWindow& w, double x, double y);
};