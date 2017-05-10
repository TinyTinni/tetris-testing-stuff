#pragma once

#include <string>
#include "../GL/GLHeader.h"
#include <boost/filesystem.hpp>


// font todo: 
// - cleanup
// - pos and width/height resolution independent
// - init
// - more flexible functions

bool loadFont(boost::filesystem::path filename);

/// Renderer call, renders all TextBoxes etc. etc.
void drawFonts(size_t windowWidth, size_t windowHeight);

class TextBox
{
    std::string m_text;
    int m_posX, m_posY;

    GLuint m_vbo;

    friend void drawFonts(size_t windowWidth, size_t windowHeight);

    void bindVBO() const { glBindBuffer(GL_ARRAY_BUFFER, m_vbo); };

public:
    TextBox();
    TextBox(const std::string& text, int x, int y);
    ~TextBox();

    void setText(const std::string& text);
    std::string text() const { return m_text; }

    /// Position in pixel space. (0,0) is upper left and (windowWidth, windowHeight) ist lower right
    void setPosition(int x, int y){ m_posX = x; m_posY = y; }
    int x() const { return m_posX; }
    int y() const { return m_posY; }

};

void setText(const std::string& text);