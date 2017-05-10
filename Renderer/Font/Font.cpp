#include "Font.h"

#include <string>
#include <fstream>
#include <sstream>

#include <map>

#pragma warning( push )
#pragma warning(disable:4996 4267)
#include "lodepng.h"
#pragma warning( pop )

#include "../GL/GLHeader.h"
#include "../GL/GLShaders.h"

#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

namespace qi = boost::spirit::qi;

struct CharDescriptor
{
    //clean 16 bytes
    unsigned short x, y;
    unsigned short Width, Height;
    float XOffset, YOffset;
    float XAdvance;
    unsigned short Page;

    CharDescriptor() : x(0), y(0), Width(0), Height(0), XOffset(0), YOffset(0),
        XAdvance(0), Page(0)
    { }
};

struct Charset
{
    unsigned short LineHeight;
    unsigned short Base;
    unsigned short Width, Height;
    unsigned short Pages;
    CharDescriptor Chars[383];
};

//BOOST_FUSION_ADAPT_STRUCT(
//    CharDescriptor,
//    (unsigned short, x)
//    (unsigned short, y)
//    (unsigned short, Height)
//    (unsigned short, Width)
//    (float, XOffset)
//    (float, YOffset)
//    (float, XAdvance)
//    (unsigned short, Page)
//    )
//
//
//template<typename Iterator>
//struct FontParser : qi::grammar<Iterator, CharDescriptor(), boost::spirit::ascii::space_type>
//{
//    FontParser() :base_type(start)
//    {
//
//    }
//
//    qi::rule<Iterator, std::string(), ascii::space_type> quoted_string;
//    qi::rule<Iterator, CharDescriptor(), ascii::space_type> start;
//};
//
//bool parseFontStream(std::istream &stream)
//{
//    return true;
//}



Charset CharsetDesc;
GLuint Program = 0;

std::multimap<int, std::pair<int, int> > kerning;

GLuint vbo = 0;
size_t textSize = 0;
GLuint fontTexture = 0;

bool loadFont(boost::filesystem::path filename)
{
    std::string Line;
    std::string Read, Key, Value;
    std::size_t i;

    filename = boost::filesystem::absolute(filename);

    std::ifstream input(filename.generic_string());

    if (!input.is_open())
    {
        std::cerr << "Cannot open font file: " << filename.generic_string() << std::endl;
    }
        

    boost::filesystem::path fontDir = filename;
    fontDir.remove_filename();

    std::string imageFile = "";

    while (!input.eof())
    {
        std::stringstream lineStream;
        std::getline(input, Line);
        lineStream << Line;

        //read the line's type
        lineStream >> Read;
        if (Read == "common")
        {
            ////this holds common data
            while (!lineStream.eof())
            {
                std::stringstream converter;
                lineStream >> Read;
                i = Read.find('=');
                Key = Read.substr(0, i);
                Value = Read.substr(i + 1);

                //assign the correct value
                converter << Value;
                if (Key == "lineHeight")
                    converter >> CharsetDesc.LineHeight;
                else if (Key == "base")
                    converter >> CharsetDesc.Base;
                else if (Key == "scaleW")
                    converter >> CharsetDesc.Width;
                else if (Key == "scaleH")
                    converter >> CharsetDesc.Height;
                else if (Key == "pages")
                    converter >> CharsetDesc.Pages;
                else if (Key == "file")
                    converter >> imageFile;
            }
        }
        else if (Read == "char")
        {
            //this is data for a specific char
            unsigned short CharID = 0;

            while (!lineStream.eof())
            {
                std::stringstream Converter;
                lineStream >> Read;
                i = Read.find('=');
                Key = Read.substr(0, i);
                Value = Read.substr(i + 1);

                //assign the correct value
                Converter << Value;
                if (Key == "id")
                    Converter >> CharID;
                else if (Key == "x")
                    Converter >> CharsetDesc.Chars[CharID].x;
                else if (Key == "y")
                    Converter >> CharsetDesc.Chars[CharID].y;
                else if (Key == "width")
                    Converter >> CharsetDesc.Chars[CharID].Width;
                else if (Key == "height")
                    Converter >> CharsetDesc.Chars[CharID].Height;
                else if (Key == "xoffset")
                    Converter >> CharsetDesc.Chars[CharID].XOffset;
                else if (Key == "yoffset")
                    Converter >> CharsetDesc.Chars[CharID].YOffset;
                else if (Key == "xadvance")
                    Converter >> CharsetDesc.Chars[CharID].XAdvance;
                else if (Key == "page")
                    Converter >> CharsetDesc.Chars[CharID].Page;
            }
        }
        else if (Read == "kerning")
        {
            int first, second, amount;

            while (!lineStream.eof())
            {
                std::stringstream Converter;
                lineStream >> Read;
                i = Read.find('=');
                Key = Read.substr(0, i);
                Value = Read.substr(i + 1);

                //assign the correct value
                Converter << Value;
                if (Key == "first")
                    Converter >> first;
                else if (Key == "second")
                    Converter >> second;
                else if (Key == "amount")
                    Converter >> amount;
            }
            kerning.insert(std::make_pair(first,std::make_pair(second, amount)));
        }
    }

    std::vector<unsigned char> pngBuffer;
    unsigned width, height;
    std::string f = fontDir.string() + imageFile;
    lodepng::decode(pngBuffer, width, height, f);

    glGenTextures(1, &fontTexture);

    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, fontTexture);

    // Give an empty image to OpenGL ( the last "0" )
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pngBuffer.data());

    // Poor filtering. Needed !
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
    return true;
}

void setText(const std::string& text)
{

    //2d pos 2d tex coords
    std::vector<GLfloat> Verts;
    Verts.resize(text.size() * 6 * 4);
    float CurX = 0.f;
    textSize = text.size();

    unsigned int j = 0;

    for (unsigned int i = 0; i < Verts.size(); ++i, ++j)
    {

        if (i > 0)
            CurX += CharsetDesc.Chars[text[j-1]].XAdvance;

        int CharX = CharsetDesc.Chars[text[j]].x;
        int CharY = CharsetDesc.Chars[text[j]].y;
        int Width = CharsetDesc.Chars[text[j]].Width;
        int Height = CharsetDesc.Chars[text[j]].Height;
        float OffsetX = CharsetDesc.Chars[text[j]].XOffset;
        float OffsetY = CharsetDesc.Chars[text[j]].YOffset;
        int kerningamount = 0;

        if (j + 1 < Verts.size())
        {
            const auto& iter = kerning.equal_range(text[j]);
            for (auto i = iter.first; i != iter.second; ++i)
                if (i->second.first == text[j + 1])
                    kerningamount = i->second.second;

        }
        CurX += kerningamount;


        //1. triangle
        //upper left
        Verts[i++] = (float)CurX + OffsetX;
        Verts[i++] = (float)OffsetY;
        Verts[i++] = (float)CharX / (float)CharsetDesc.Width;
        Verts[i++] = (float)CharY / (float)CharsetDesc.Height;
        

        //upper right
        Verts[i++] = (float)Width + CurX + OffsetX;
        Verts[i++] = (float)OffsetY;
        Verts[i++] = (float)(CharX + Width) / (float)CharsetDesc.Width;
        Verts[i++] = (float)CharY / (float)CharsetDesc.Height;
              
              
        // lower right
        Verts[i++] = (float)Width + CurX + OffsetX;
        Verts[i++] = (float)Height + OffsetY;
        Verts[i++] = (float)(CharX + Width) / (float)CharsetDesc.Width;
        Verts[i++] = (float)(CharY + Height) / (float)CharsetDesc.Height;



        //2. triangle

        //upper left
        Verts[i++] = (float)CurX + OffsetX;
        Verts[i++] = (float)OffsetY;
        Verts[i++] = (float)CharX / (float)CharsetDesc.Width;
        Verts[i++] = (float)CharY / (float)CharsetDesc.Height;

        // lower right
        Verts[i++] = (float)Width + CurX + OffsetX;
        Verts[i++] = (float)Height + OffsetY;
        Verts[i++] = (float)(CharX + Width) / (float)CharsetDesc.Width;
        Verts[i++] = (float)(CharY + Height) / (float)CharsetDesc.Height;
        
              
        //lower left
        Verts[i++] = (float)CurX + OffsetX;
        Verts[i++] = (float)Height + OffsetY;
        Verts[i++] = (float)CharX / (float)CharsetDesc.Width;
        Verts[i] = (float)(CharY + Height) / (float)CharsetDesc.Height;
        
    }

    if (!vbo)
        glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, Verts.size()* sizeof(GLfloat), Verts.data(), GL_DYNAMIC_DRAW);
}

std::vector<TextBox*> textBoxes;

void drawFonts(size_t windowWidth, size_t windowHeight)
{
    if (!Program)
    {
        Program = glCreateProgram();
        compileLinkProgram<shader::vertex::FontShader, shader::fragment::FontShader>(Program);
        //compileLinkProgram(Program, "Shaders//Font.vp", "Shaders//Font.fp");
    }

    glUseProgram(Program); // TODO

    //set uniforms
    //const GLuint projUniform = glGetUniformLocation(m_quadProgram, "uProjection");
    //glUniformMatrix4fv(projUniform, 1, GL_FALSE, &m_projectionMatrix.data()[0]);

    // enable states
    //glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
    glEnable(GL_BLEND);

    //draw buffer
    
    const GLuint renderTargetUniform = glGetUniformLocation(Program, "uTexture");
    glUniform1i(renderTargetUniform, 0);

    const GLuint windowWidthUniform = glGetUniformLocation(Program, "uWindowWidth");
    glUniform1ui(windowWidthUniform, static_cast<GLuint>(windowWidth));
    const GLuint windowHeightUniform = glGetUniformLocation(Program, "uWindowHeight");
    glUniform1ui(windowHeightUniform, static_cast<GLuint>(windowHeight));

    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, fontTexture);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    for (const auto& text : textBoxes)
    {
        text->bindVBO();
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);//Position
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), reinterpret_cast<void*>(2 * sizeof(GLfloat)));//Color

        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(textSize * 6));
    }

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    // disable states
    glDisable(GL_BLEND);
}

TextBox::TextBox()
: TextBox("",0,0)
{
    
}

TextBox::TextBox(const std::string& text, int x, int y)
:m_text(text), m_posX(x), m_posY(y), m_vbo(0)
{
    textBoxes.push_back(this);
    glGenBuffers(1, &m_vbo);
}

TextBox::~TextBox()
{
    glDeleteBuffers(1, &m_vbo);
    textBoxes.erase(std::find(textBoxes.begin(),textBoxes.end(),this));
}

void TextBox::setText(const std::string& text)
{
    m_text = text;
    //2d pos 2d tex coords
    std::vector<GLfloat> Verts;
    Verts.resize(text.size() * 6 * 4);
    float CurX = 0.f;
    textSize = text.size();

    unsigned int j = 0;

    for (unsigned int i = 0; i < Verts.size(); ++i, ++j)
    {

        if (i > 0)
            CurX += CharsetDesc.Chars[text[j - 1]].XAdvance;

        int CharX = CharsetDesc.Chars[text[j]].x;
        int CharY = CharsetDesc.Chars[text[j]].y;
        int Width = CharsetDesc.Chars[text[j]].Width;
        int Height = CharsetDesc.Chars[text[j]].Height;
        float OffsetX = CharsetDesc.Chars[text[j]].XOffset;
        float OffsetY = CharsetDesc.Chars[text[j]].YOffset;
        int kerningamount = 0;

        if (j + 1 < Verts.size())
        {
            const auto& iter = kerning.equal_range(text[j]);
            for (auto i = iter.first; i != iter.second; ++i)
                if (i->second.first == text[j + 1])
                    kerningamount = i->second.second;

        }
        CurX += kerningamount;


        //1. triangle
        //upper left
        Verts[i++] = (float)CurX + OffsetX;
        Verts[i++] = (float)OffsetY;
        Verts[i++] = (float)CharX / (float)CharsetDesc.Width;
        Verts[i++] = (float)CharY / (float)CharsetDesc.Height;


        //upper right
        Verts[i++] = (float)Width + CurX + OffsetX;
        Verts[i++] = (float)OffsetY;
        Verts[i++] = (float)(CharX + Width) / (float)CharsetDesc.Width;
        Verts[i++] = (float)CharY / (float)CharsetDesc.Height;


        // lower right
        Verts[i++] = (float)Width + CurX + OffsetX;
        Verts[i++] = (float)Height + OffsetY;
        Verts[i++] = (float)(CharX + Width) / (float)CharsetDesc.Width;
        Verts[i++] = (float)(CharY + Height) / (float)CharsetDesc.Height;



        //2. triangle

        //upper left
        Verts[i++] = (float)CurX + OffsetX;
        Verts[i++] = (float)OffsetY;
        Verts[i++] = (float)CharX / (float)CharsetDesc.Width;
        Verts[i++] = (float)CharY / (float)CharsetDesc.Height;

        // lower right
        Verts[i++] = (float)Width + CurX + OffsetX;
        Verts[i++] = (float)Height + OffsetY;
        Verts[i++] = (float)(CharX + Width) / (float)CharsetDesc.Width;
        Verts[i++] = (float)(CharY + Height) / (float)CharsetDesc.Height;


        //lower left
        Verts[i++] = (float)CurX + OffsetX;
        Verts[i++] = (float)Height + OffsetY;
        Verts[i++] = (float)CharX / (float)CharsetDesc.Width;
        Verts[i] = (float)(CharY + Height) / (float)CharsetDesc.Height;

    }

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, Verts.size()* sizeof(GLfloat), Verts.data(), GL_STATIC_DRAW);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}