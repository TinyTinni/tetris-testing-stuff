#pragma once

/**
 \brief class controlling standard keyboard actions for the window
 esc: close program
 alt + enter: toogle fullscreen
*/

class MainWindow;

void stdKeyLayout(MainWindow& mw, int key, int scancode, int action,  int modifier);