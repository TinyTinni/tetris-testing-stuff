#pragma once

#include <functional>
#include <vector>

struct GLFWwindow;
struct GLFWmonitor;

#include "Control/xBox360.h"

class MainWindow
{
    GLFWwindow* m_window;
    std::function<void ()> m_renderFunction;
    typedef std::function<void(MainWindow&, int, int, int, int)> KeyboardCallback;
    std::vector<KeyboardCallback> m_keyboardCallbacks;
    typedef std::function<void(unsigned, unsigned)> ResizeCallback;
    std::vector<ResizeCallback> m_resizeCallbacks;
    typedef std::function<void(MainWindow&, int, int, int)> MouseButtonCallback;
    std::vector<MouseButtonCallback> m_mouseButtonCallbacks;
    typedef std::function<void(MainWindow&, double, double)>  MousePosCallback;
    std::vector<MousePosCallback> m_mousePosCallbacks;

    /// creates window and register all callbacks and data and makes thw windows context current
    GLFWwindow* createWindow(unsigned width, unsigned height, const char* title ,GLFWmonitor* mon, GLFWwindow* share);

    // callback functions
    friend void keyBoardCallbackFunction(GLFWwindow *, int, int, int, int);
    friend void resizeCallbackFunction(GLFWwindow*, int, int);
    friend void mousePosCallbackFunction(GLFWwindow* window, double x, double y);
    friend void mouseButtonCallbackFunction(GLFWwindow* window, int button, int action, int mods);

public:
    /// creates window and opengl context. if fullscreen is set, window resolution is screen resolution 
    MainWindow(unsigned width, unsigned height, bool fullscreen);
    ~MainWindow();

    /// change to fullscreen or from fullscreen. If fullscreen is selected, window resolution is screen resolution
    void fullscreen(bool fullscreenOn, unsigned with=640, unsigned height=480);
    bool isFullscreen();

    void resize(const unsigned width,const unsigned height);
    unsigned height();
    unsigned width();

    void enterMainLoop();
    void onRender(const std::function<void ()> &func){m_renderFunction = func;}
    void enterMainLoop(const std::function<void ()> &renderFunc){m_renderFunction= renderFunc; enterMainLoop();}
    void leaveMainLoop();

    /** register a function which will be called if a keystate has changed.
    You can register more than one function
    first is the window whre the event occurred
    second param of this function recives the glfw keycode (see http://www.glfw.org/docs/latest/group__keys.html)
    third system specific scancode
    fourth is event type 	GLFW_PRESS, GLFW_RELEASE or GLFW_REPEAT
    last is modifier type GLFW_MOD_SHIFT, GLFW_MOD_CONTROL, GLFW_MOD_ALT or GLFW_MOD_SUPER  
    */
    void setKeyboardCallback(KeyboardCallback cb){ m_keyboardCallbacks.emplace_back(cb); }

    /** register function that is called when window is resized
    *   first paramater is new width without decoration (edges/border)
    *   second parameter ist new height without decoration (edges/border)
    */
    void setResizeCallback(ResizeCallback cb){ m_resizeCallbacks.emplace_back(cb); }

    void setMouseButtonCallback(MouseButtonCallback cb){ m_mouseButtonCallbacks.emplace_back(cb); }

    void setMousePosCallback(MousePosCallback cb){ m_mousePosCallbacks.emplace_back(cb); }

    GLFWwindow* getGLFWWindowPtr(){ return m_window; }

};