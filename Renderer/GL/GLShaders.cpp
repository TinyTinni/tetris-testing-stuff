#include "GLShaders.h"

#include <fstream>
#include <iostream>
#include <stdexcept>

static std::string getFile(const std::string &filename);
static bool checkShader(const GLuint& shader, const std::string &filename);
static bool checkProgram(const GLuint& program);

static std::string getFile(const std::string &filename)
{

    std::ifstream file(filename);
    if (!file.is_open())
        throw std::invalid_argument("Could not open file: " + filename);

    
    file.seekg(0, std::ios::end);
    std::string result(static_cast<size_t>(file.tellg()),'\0');
    file.seekg(0, std::ios::beg);
    file.read(&result[0], result.size());

    return result;
}

static bool checkShader(const GLuint& shader, const std::string &filename)
{
    GLint status = GL_FALSE;

    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if ( !status ){
        GLint InfoLogLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &InfoLogLength);
        std::string errorlog(InfoLogLength,'\0');
        glGetShaderInfoLog(shader, InfoLogLength, NULL, &errorlog[0]);
        std::cerr << "shader error in file " << filename << ":\n" << errorlog << std::endl;
        return false;
    }
    return true;
}

bool checkProgram(const GLuint& program)
{
    GLint status = GL_FALSE;

    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if ( !status ){
        GLint InfoLogLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &InfoLogLength);
        std::string errorlog(InfoLogLength,'\0');
        glGetShaderInfoLog(program, InfoLogLength, NULL, &errorlog[0]);
        std::cerr << "program error: " << errorlog << std::endl;
        return false;
    }
    return true;
}

void compileLinkProgramFromSource(const GLuint& program, const std::string &vertex, const std::string &geometry, const std::string &fragment)
{
    GLuint shader = glCreateShader(GL_GEOMETRY_SHADER);
    const char* codePtr = geometry.c_str();
    glShaderSource(shader, 1, &codePtr, NULL);
    glCompileShader(shader);
    checkShader(shader, geometry);
    if (!checkShader(shader, geometry))
        throw std::invalid_argument("creating geometry shader failed");

    glAttachShader(program, shader);

    compileLinkProgramFromSource(program, vertex, fragment);

    glDeleteShader(shader);

}

void compileLinkProgramFromSource(const GLuint& program, const std::string &vertex, const std::string &fragment)
{
    GLuint vp = glCreateShader(GL_VERTEX_SHADER);
    const char* codePtr = vertex.c_str();
    glShaderSource(vp, 1, &codePtr, NULL);
    glCompileShader(vp);
    if (!checkShader(vp, vertex))
        throw std::invalid_argument("creating vertex shader failed");

    GLuint fp = glCreateShader(GL_FRAGMENT_SHADER);
    codePtr = fragment.c_str();
    glShaderSource(fp, 1, &codePtr, NULL);
    glCompileShader(fp);
    checkShader(fp, fragment);
    if (!checkShader(fp, fragment))
       throw std::invalid_argument("creating fragment shader failed");

    glAttachShader(program, fp);
    glAttachShader(program, vp);

    glLinkProgram(program);
    checkProgram(program);    
    if (!checkProgram(program))
        throw std::invalid_argument("creating fragment program failed");

    glDeleteShader(fp);
	glDeleteShader(vp);

}


void compileLinkProgramFromFile(const GLuint& program, const std::string &vertex, const std::string &fragment)
{
    std::string vertexCode = getFile(vertex);
    std::string fragmentCode = getFile(fragment);
    compileLinkProgramFromSource(program, vertexCode, fragmentCode);
}

void compileLinkProgramFromFile(const GLuint& program, const std::string &vertex, const std::string &geometry, const std::string &fragment)
{
    std::string vertexCode = getFile(vertex);
    std::string fragmentCode = getFile(fragment);
    std::string geometryCode = getFile(geometry);
    compileLinkProgramFromSource(program, vertexCode,geometry, fragmentCode);
}

