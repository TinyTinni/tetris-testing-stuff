#include "Window.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <limits>
#include <stdexcept>
#include <algorithm>

#include <utility/TweakBar.h>

//---------------------------Callbacks-----------------------
void errorCallback(int error, const char *str)
{
    std::cerr << "GLFW Error #" << error << ": " << str << std::endl;
}

void keyBoardCallbackFunction(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (tw::GLFW::eventKey(window,key,scancode, action,mods))
        return;

    MainWindow* mw = reinterpret_cast<MainWindow*>(glfwGetWindowUserPointer(window));
    for (auto func : mw->m_keyboardCallbacks)
        func(*mw, key, scancode, action, mods);
}

void resizeCallbackFunction(GLFWwindow* window, int width, int height)
{
    MainWindow* mw = reinterpret_cast<MainWindow*>(glfwGetWindowUserPointer(window));
    for (auto func : mw->m_resizeCallbacks)
        func(width, height);

    tw::mainWindowResized(width, height);
}

void mousePosCallbackFunction(GLFWwindow* window, double x, double y)
{
    if (tw::GLFW::mouseMotion(window, x, y))
        return;

    MainWindow* mw = reinterpret_cast<MainWindow*>(glfwGetWindowUserPointer(window));
    for (auto func : mw->m_mousePosCallbacks)
        func(*mw, x, y);
}

void mouseButtonCallbackFunction(GLFWwindow* window, int button, int action, int mods)
{
    if (tw::GLFW::eventMouseButton(window, button, action, mods))
        return;

    MainWindow* mw = reinterpret_cast<MainWindow*>(glfwGetWindowUserPointer(window));
    for (auto func : mw->m_mouseButtonCallbacks)
        func(*mw, button, action, mods);
}

//---------------------------Window Creation-----------------------
GLFWwindow* MainWindow::createWindow(unsigned width, unsigned height, const char* title ,GLFWmonitor* mon, GLFWwindow* share)
{
    GLFWwindow* w = glfwCreateWindow(width,height,title,mon,share);
    if (!w)
    {
        errorCallback(GLFW_PLATFORM_ERROR,"Could not create Window");
        glfwTerminate();
    }
    glfwSetWindowUserPointer(w, reinterpret_cast<void *>(this));
    glfwSetKeyCallback(w,keyBoardCallbackFunction);
    glfwMakeContextCurrent(w);
    glfwSetWindowSizeCallback(w,resizeCallbackFunction);
    glfwSetCursorPosCallback(w, mousePosCallbackFunction);
    glfwSetMouseButtonCallback(w, mouseButtonCallbackFunction);

    glfwSetCharCallback(w,(GLFWcharfun)tw::GLFW::eventChar);   
    glfwSetScrollCallback(w, (GLFWscrollfun)tw::GLFW::mouseWheel);

    return w;
}

MainWindow::MainWindow(unsigned width, unsigned height, bool fullscreen):
m_window(0)
{
    if (!glfwInit ())
        throw std::runtime_error("Could not initialize glfw");

#if _DEBUG
    // Create a debug OpenGL context or tell your OpenGL library (GLFW, SDL) to do so.
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif
    //support for opengl 3.3
    if (GLFW_CONTEXT_VERSION_MAJOR < 3)
        throw std::runtime_error("OpenGL Version lower than 3.3 is not supported");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES,4);

    //enable VSync
    glfwSwapInterval(-1);

    // Create a windowed mode window and its OpenGL context
    GLFWmonitor* primaryMonitor = nullptr;
    if (fullscreen) 
    {
        primaryMonitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* vm = glfwGetVideoMode(primaryMonitor);
        width = vm->width;
        height = vm->height;
    }

    m_window = createWindow(width, height, "", primaryMonitor, NULL);
    if (!m_window)
        throw std::runtime_error("Could not create window");

    // Set global glfw error handling (here to get rid of the error message at the beginning)
    glfwSetErrorCallback(errorCallback);

    tw::init(tw::GraphicAPI::TW_OPENGL_CORE);
    tw::mainWindowResized(width, height);
}

MainWindow::~MainWindow()
{
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

void MainWindow::fullscreen(bool fullscreenOn, unsigned width, unsigned height)
{
    GLFWmonitor* primaryMonitor = nullptr;
    if (fullscreenOn) 
    {
        primaryMonitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* vm = glfwGetVideoMode(primaryMonitor);
        width = vm->width;
        height = vm->height;
    }
    GLFWwindow* tmpwindow = createWindow(width, height, "", primaryMonitor, m_window);
    // notify about resizing
    resizeCallbackFunction(tmpwindow,width,height);
    glfwDestroyWindow(m_window);
    m_window = tmpwindow;

    tw::mainWindowResized(width, height);
}

bool MainWindow::isFullscreen()
{
    return glfwGetWindowMonitor(m_window) != 0;
}

//---------------------------Main Loop-----------------------
void MainWindow::enterMainLoop()
{
#ifdef TIME_MEASURE
    std::chrono::time_point<std::chrono::high_resolution_clock> frameTime;
    std::chrono::time_point<std::chrono::high_resolution_clock> lastSecond = std::chrono::high_resolution_clock::now();
    std::chrono::milliseconds::rep minframe = std::numeric_limits<long long>::max(), maxframe = 0;

    unsigned frames = 0;
    std::stringstream timeStr;
    timeStr << std::ios::fixed;
#endif
    // Loop until the user closes the window
    while (!glfwWindowShouldClose(m_window))
    {
#ifdef TIME_MEASURE
        frameTime = std::chrono::high_resolution_clock::now();
#endif
        // Render here
        glfwMakeContextCurrent(m_window);
        m_renderFunction();

        //draw tweakBar
        tw::draw();

        // Swap front and back buffers
        glfwSwapBuffers(m_window);

        // Poll for and process events
        glfwPollEvents();

#ifdef TIME_MEASURE
        /* write time per frame (each frame) and fps (each second)
        string layout:
        0-5 : timer per frame
        6-17: fps
        18-: minmax avg
        */
        ++frames;
        std::chrono::milliseconds frame_timediff(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()-frameTime).count());
        minframe = std::min(minframe, frame_timediff.count());
        maxframe = std::max(maxframe, frame_timediff.count());
        timeStr << std::setprecision(2) << frame_timediff.count() << " ms | "; // 6 chars
        std::chrono::milliseconds seconds(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - lastSecond).count());
        if (seconds.count() >= 1000) 
        {
            timeStr.str("");
            timeStr << std::setprecision(2) << frame_timediff.count() << " ms | "; // 6 chars
            timeStr << std::setprecision(4) << 1000*frames/static_cast<float>(seconds.count()) <<" fps | "; // 12 chars
            timeStr << std::setprecision(2) << "max: " << maxframe << "ms | min: " << minframe << "ms | avg: " << 1000.0/frames << "ms | ";

            lastSecond = std::chrono::high_resolution_clock::now();
            frames = 0;
            maxframe = 0;
            minframe = std::numeric_limits<long long>::max();
        }
        glfwSetWindowTitle(m_window,timeStr.str().c_str());
        timeStr.seekp(std::ios::beg);
#endif
    }
    
}

void MainWindow::leaveMainLoop()
{
    tw::terminate();
    glfwSetWindowShouldClose(m_window,1);
}

void MainWindow::resize(const unsigned width,const unsigned height)
{
    glfwSetWindowSize(m_window,width,height);
}

unsigned MainWindow::height()
{
    int width = 0,height = 0;
    glfwGetWindowSize(m_window,&width,&height);
    return height;
}

unsigned MainWindow::width()
{
    int width = 0,height = 0;
    glfwGetWindowSize(m_window,&width,&height);
    return width;
}