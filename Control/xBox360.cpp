#include "xBox360.h"
#include <boost/math/special_functions/round.hpp>
#include <GLFW/glfw3.h>
#include <utility/TweakBar.h>

/*
button a = 0
button b = 1
button x = 2
button y = 3
LB = 4
RB = 5
select = 6
start = 7
stick left click = 8
stick right click = 9
pad_up = 10
pad_right = 11
pad_down = 12
pad_left = 13

axis:
stick left x = 0 (left = -1, right = 1)
stick left y = 1 (down = 1, up = -1)
LT = axis 2 +1
RT = axis 2 -1
stick right x = 4 (left = -1, right = 1)
stick right y = 3 (down = 1, up = -1)
*/

const unsigned BUTTON_A = 0;
const unsigned BUTTON_B = 1;
const unsigned BUTTON_X = 2;
const unsigned BUTTON_Y = 3;
const unsigned BUTTON_SELECT = 6;
const unsigned BUTTON_START = 7;

bool XBoxControl::buttonPressed(const unsigned buttonId, const unsigned char *currentStates)
{
    bool result = (currentStates[buttonId]) && (!m_buttonStates[buttonId]);
    m_buttonStates[buttonId] = currentStates[buttonId];
    return result;
}

XBoxControl::XBoxControl(Controller& cont):
    m_controller(cont), 
    m_buttonStates(), m_axisTimes(), m_lastTime(std::chrono::high_resolution_clock::now())
{
    int countButtons;
    glfwGetJoystickButtons(0, &countButtons);
    m_buttonStates.resize(countButtons, 0);

    int countAxis;
    glfwGetJoystickAxes(0, &countAxis);
    m_axisTimes.resize(countAxis, 0.f);
}

void XBoxControl::ProcessAxisStates()
{
    int countAxis;
    const float *axisState = glfwGetJoystickAxes(0, &countAxis);

    if (!axisState)
        return;

    auto now = std::chrono::high_resolution_clock::now();
    auto timeDelta = now - m_lastTime;

    static float sensitivity = 2.75f;
    static bool init = false;
    if (!init)
    {
        tw::globalTweakBar().addVarRW("gamepad_sensitivity", tw::Type::TW_TYPE_FLOAT, &sensitivity, "min=0.1 max=10.0 step=0.01");
        init = true;
    }

    const float multi = 0.0000000005f * sensitivity;
    


    if (axisState[0] <= -0.05f)
    {
        if (m_axisTimes[0] > 0)
            m_axisTimes[0] = 0;

        m_axisTimes[0] -= multi*axisState[0] * axisState[0] * timeDelta.count();

        if (m_axisTimes[0] <= -1.f)
        {
            m_controller.translateLeft();
            m_axisTimes[0] = 0.f;
        }
            
    }
    else if (axisState[0] >= 0.05f)
    {
        if (m_axisTimes[0] < 0)
            m_axisTimes[0] = 0;

        m_axisTimes[0] += multi*axisState[0] * axisState[0] * timeDelta.count();

        if (m_axisTimes[0] >= 1.f)
        {
            m_controller.translateRight();
            m_axisTimes[0] = 0.f;
        }
            
    }
    else
        rightP = true;
    if (axisState[1] >= 0.15f)
    {
        if (downP)
        {
            m_controller.translateDown();
            downP = false;
        }

    }
    else
        downP = true;
    if (abs(axisState[2]) >= 0.5f)
    {
        if (rotateP)
        {
            m_controller.rotateZ(static_cast<int>(boost::math::round(axisState[2])));
            rotateP = false;
        }

    }
    else
        rotateP = true;

}

void XBoxControl::ProcessButtonStates()
{
    int countButtons;
    const unsigned char *buttonState = glfwGetJoystickButtons(0, &countButtons);
    if (!buttonState)
        return;

    if (buttonPressed(BUTTON_START,buttonState))
    {
        if (m_controller.isPaused())
            m_controller.start();
        else
            m_controller.pause();
    }
}

void XBoxControl::pollEvent()
{    
    ProcessButtonStates();
    ProcessAxisStates();
}