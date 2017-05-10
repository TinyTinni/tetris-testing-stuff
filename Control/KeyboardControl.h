#pragma once
#include "Controller.h"

class KeyboardControl
{
    Controller& m_controler;

public:
    KeyboardControl(Controller& cont) :m_controler(cont){}
    
    void keyboardGlfwCallback(MainWindow& mw, int key, int scancode, int action,  int modifier); 
};