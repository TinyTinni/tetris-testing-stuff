#include "GLErrorHandling.h"

#include <string>
#include <iostream>
#include <sstream>

#include "GLHeader.h"

#ifdef WIN32
#include <Windows.h>
#elif !defined(APIENTRY)
#define APIENTRY
#endif

//////////////////////////////////////////////////////
#ifdef USE_CG
#include <Cg/cg.h>
CGcontext g_context = 0;
static void cgErrorCallback();
void registerCgContext(_CGcontext *context){g_context = context;}
#endif

//////////////////////////////////////////////////////
static std::string FormatDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, const char* msg) ;
void APIENTRY OpenGLDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const GLvoid* userParam);

//////////////////////////////////////////////////////
void registerGLErrorCallbacks()
{

#if _DEBUG 
    // Register your callback function if supported
    if (!checkExtension("GL_KHR_debug"))
    {
        // TODO: load function for GL_ARB_debug and GL_AMD_debug
        std::cout << "No Debug Callback available" << std::endl;
        return;
    }
    glDebugMessageCallback(OpenGLDebugCallback, NULL);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

    // Cg error callback if used
    #ifdef USE_CG
    cgSetErrorCallback(cgErrorCallback);
    #endif
#endif
 
}
//////////////////////////////////////////////////////
std::string FormatDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, const char* msg) {
    std::stringstream stringStream;
    std::string sourceString;
    std::string typeString;
    std::string severityString;
 
    // The AMD variant of this extension provides a less detailed classification of the error,
    // which is why some arguments might be "Unknown".
    switch (source) {
        case GL_DEBUG_SOURCE_API: {
            sourceString = "API";
            break;
        }
        case GL_DEBUG_SOURCE_APPLICATION: {
            sourceString = "Application";
            break;
        }
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM: {
            sourceString = "Window System";
            break;
        }
        case GL_DEBUG_SOURCE_SHADER_COMPILER: {
            sourceString = "Shader Compiler";
            break;
        }
        case GL_DEBUG_SOURCE_THIRD_PARTY: {
            sourceString = "Third Party";
            break;
        }
        case GL_DEBUG_SOURCE_OTHER: {
            sourceString = "Other";
            break;
        }
        default: {
            sourceString = "Unknown";
            break;
        }
    }
 
    switch (type) {
        case GL_DEBUG_TYPE_ERROR: {
            typeString = "Error";
            break;
        }
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: {
            typeString = "Deprecated Behavior";
            break;
        }
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: {
            typeString = "Undefined Behavior";
            break;
        }
        case GL_DEBUG_TYPE_PORTABILITY: {
            typeString = "Portability";
            break;
        }
        case GL_DEBUG_TYPE_PERFORMANCE: {
            typeString = "Performance";
            break;
        }
        case GL_DEBUG_TYPE_OTHER: {
            typeString = "Other";
            break;
        }
        default: {
            typeString = "Unknown";
            break;
        }
    }
 
    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH: {
            severityString = "High";
            break;
        }
        case GL_DEBUG_SEVERITY_MEDIUM: {
            severityString = "Medium";
            break;
        }
        case GL_DEBUG_SEVERITY_LOW: {
            severityString = "Low";
            break;
        }
        default: {
            severityString = "Unknown";
            break;
        }
    }
 
    stringStream << "OpenGL Error: " << msg;
    stringStream << " [Source = " << sourceString;
    stringStream << ", Type = " << typeString;
    stringStream << ", Severity = " << severityString;
    stringStream << ", ID = " << id << "]";
 
    return stringStream.str();
}
 //////////////////////////////////////////////////////
void APIENTRY OpenGLDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei /*length*/, const GLchar* message, const GLvoid* /*userParam*/) {
    std::string error = FormatDebugOutput(source, type, id, severity, message);
    std::cout << error << std::endl;
}

//////////////////////////////////////////////////////
#ifdef USE_CG
void cgErrorCallback()
{
    CGerror error;
    const char *string = cgGetLastErrorString(&error);

    std::cerr << "CG Error #" << error << ": "<< string << std::endl;
    if (error == CG_COMPILER_ERROR && g_context)
    {
        std::cerr << cgGetLastListing(g_context) << std::endl; 
    }
    throw CGexception(string);
}
#endif
