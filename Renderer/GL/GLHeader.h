#pragma once

#include "gl_core_3_3.h"
#include <stdexcept>
#include <string>
#include <cstring>

class GLLoadException : public std::runtime_error
{
public:
    GLLoadException(const std::string& what_arg):runtime_error(what_arg){}
};


inline bool checkExtension(const char* str)
{
    GLint max = 0;
    glGetIntegerv(GL_NUM_EXTENSIONS, &max);
    for (int i = 0; i < max; ++i)
        if (!strcmp(reinterpret_cast<const char*>(glGetStringi(GL_EXTENSIONS, i)), str))
            return true;
    return false;
}

#ifdef _WIN32
#include <windows.h>
inline PROC  WINAPI glGetProcAddress(const char* str)
{
    PROC result = wglGetProcAddress(str); 
    if (!result) throw GLLoadException(std::string("Was not able to load function: ")+std::string(str));
    return result;
}

#else

#endif