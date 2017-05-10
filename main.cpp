#include <cstring>
#include <cstdlib>
#include "Window.h"

#include "Control/StdKeyboard.h"
#include "Control/KeyboardControl.h"
#include "Control/Mouse.h"
#include "Control/xBox360.h"
#include "Control/MSKinect.h"

#include "Renderer/GLRenderer.h"
#include "Data/SimpleTetris.h"

#include "Renderer/Screen.h"

#include <iostream>
#include <exception>

#include <GLFW/glfw3.h>

struct StartupOptions
{
    bool fullscreen;
    StartupOptions():fullscreen(false){};
};

StartupOptions analyzeArguments(int argc, char* argv[])
{
    StartupOptions so;
    for (int i=0; i < argc && !so.fullscreen; ++i)
        so.fullscreen |= !strncmp(argv[i],"-fullscreen",11);

    return so;
}

int main(int argc, char *argv[])
{
   // try
    {
        StartupOptions options = analyzeArguments(argc,argv);

        MainWindow window(640,480,options.fullscreen);

        window.setKeyboardCallback(stdKeyLayout);

        GLRenderer renderer(window.width(),window.height());
        window.setResizeCallback([&renderer](unsigned w, unsigned h){renderer.resize(w,h);});

        /*TESTING QUAD SYSTEM*/
        /*
        QuadEntity qg(3);
        qg[0].setColor( 0.0f,0.0f,1.f);
        qg[0].setPosition(  0.0f, 0.55f, -2.0f);
        qg[1].setColor( 1.f,0.0f,0.f);
        qg[1].setPosition( 0.0f, 0.5f, -2.0f);
        qg[2].setColor( 0.f,1.0f,0.f);
        qg[2].setPosition(  0.0f, 0.45f, -2.0f);
        renderer.addQuadEntity(qg);

        QuadEntity qg2(5);
        qg2[0].setColor( 0.0f,0.0f,1.f);
        qg2[0].setPosition(  0.0f, 0.55f, -2.0f);
        qg2[1].setColor( 1.f,0.0f,0.f);
        qg2[1].setPosition( 0.0f, 0.5f, -2.0f);
        qg2[2].setColor( 0.f,1.0f,0.f);
        qg2[2].setPosition(  0.0f, 0.45f, -2.0f);
        qg2[3].setColor( 0.f,0.0f,1.f);
        qg2[3].setPosition(  0.0f, 0.4f, -2.0f);
        qg2[4].setColor( 0.f,1.0f,0.f);
        qg2[4].setPosition(  0.0f, 0.6f, -2.0f);
        renderer.addQuadEntity(qg2);

        qg.translate(0.f,-0.5f,0.f);

        std::array<float,3> origin = {0.0f,0.0f,-2.0f};
        qg2.translate(-0.05f,0.f,0.f);

        qg2.rotateZ(1);
        qg.clear();

        QuadEntity qg3(3);
        qg3[0].setColor( 0.0f,0.0f,1.f);
        qg3[0].setPosition(  -0.5f, 0.55f, -2.0f);
        qg3[1].setColor( 1.f,0.0f,0.f);
        qg3[1].setPosition( -0.5f, 0.5f, -2.0f);
        qg3[2].setColor( 0.f,1.0f,0.f);
        qg3[2].setPosition(  -0.5f, 0.45f, -2.0f);

        renderer.addQuadEntity(qg3);

        std::cout << "lower bound: " << qg2.lowerBound(-0.05f);

        //qg2.clear();
        //qg.clear();
        //qg3.clear();
        */
        /*********************/

		Screen test;
        SimpleTetris game(&renderer);
        Controller ctrl(game,0);
        KeyboardControl khb(ctrl);
        MouseControl mctl(ctrl);
        KinectControl kinect (ctrl);
        XBoxControl* xBoxControl = nullptr;
        window.setKeyboardCallback([&khb](MainWindow& mw, int key, int scancode, int action,  int modifier){khb.keyboardGlfwCallback(mw,key,scancode,action,modifier);});
        window.setMouseButtonCallback([&mctl](MainWindow& mw, int a, int b, int c){mctl.mouseButtonGlfwCallback(mw, a, b, c); });
        window.setMousePosCallback([&mctl](MainWindow& mw, double x, double y){mctl.mousePosGlfwCallback(mw, x, y); });

        window.enterMainLoop([&renderer, &game, &ctrl, &xBoxControl, &kinect](){
           
            if (!xBoxControl && glfwJoystickPresent(0))
            {
                std::cout << "Controller Present:\n" << glfwGetJoystickName(0) << std::endl;
                xBoxControl = new XBoxControl(ctrl);
            }
            kinect.pollEvents();
            if (xBoxControl)
                xBoxControl->pollEvent();

			renderer.render();
			game.runGame();
        });
    } /*catch (std::exception &e)
    {
        std::cerr << "EXCEPTION: " << e.what() << std::endl;
#if (defined(_WIN32) && defined(_DEBUG))
        system("Pause");
#endif
    }*/

    return EXIT_SUCCESS;
}
