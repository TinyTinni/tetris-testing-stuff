#pragma once

#include <string>
#include "GLHeader.h"
#include "../ShaderCollection.h"

/**
* \brief compile shaders and link it to the current program
* @param program has to be an valid and already created glProgrma (glCreateProgram)
* @param vertex has to be a valid path to vertex shader. throws std::invalid_argument if an error occurs
* @param fragment has to be a valid path to fragment shader. throws std::invalid_argument if an error occurs
*/

template<typename Vertex, typename Geometry, typename Fragment>
inline void compileLinkProgram(const GLuint& program)
{
    std::string vertexCode = Vertex::getCode();
    std::string geometryCode = Geometry::getCode();
    std::string fragmentCode = Fragment::getCode();
    compileLinkProgramFromSource(program, vertexCode, geometryCode, fragmentCode);
}

template<typename Vertex, typename Fragment>
inline void compileLinkProgram(const GLuint& program)
{
    std::string vertexCode = Vertex::getCode();
    std::string fragmentCode = Fragment::getCode();
    compileLinkProgramFromSource(program, vertexCode, fragmentCode);
}

void compileLinkProgramFromSource(const GLuint& program, const std::string &vertex, const std::string &fragment);

void compileLinkProgramFromSource(const GLuint& program, const std::string &vertex, const std::string &geometry, const std::string &fragment);

void compileLinkProgramFromFile(const GLuint& program, const std::string &vertex, const std::string &fragment);

void compileLinkProgramFromFile(const GLuint& program, const std::string &vertex, const std::string &geometry, const std::string &fragment);
