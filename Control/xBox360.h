#pragma once
#include "Controller.h"

#include <vector>
#include <chrono>

class XBoxControl
{
    Controller& m_controller;
    bool leftP;
    bool rightP;
    bool downP;
    bool rotateP;

    std::vector<unsigned char> m_buttonStates;
    std::vector<float> m_axisTimes;
    const float resetTime = 1.f;

    std::chrono::high_resolution_clock::time_point m_lastTime;

private:
    void ProcessAxisStates();
    void ProcessButtonStates();
    bool buttonPressed(const unsigned buttonId, const unsigned char *currentStates);

public:
    XBoxControl(Controller& cont);

    void pollEvent();
};