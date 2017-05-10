#pragma once
/**

    registers error callbacks for opengl and cg (define USE_CG)
    to do this, simply call "registerErrorCallbacks"

    - cg errors will also throw an exception on error: CGexception
    - to get more information for compile error when cg shaders are compiled, you have
        to register your context with 

*/

/// registers opengl error callbacks
void registerGLErrorCallbacks();


/***** cg error handling *****/
#if (defined(CG_VERSION_NUM) || defined(USE_CG))
#ifndef USE_CG
    #define USE_CG
#endif
struct _CGcontext;

/// use specific context for more detailed compile errors
void registerCgContext(_CGcontext *context);



#include <exception>
class CGexception: public std::exception
{
    const char* w;
public:
  CGexception(const char* what):w(what){}
  const char* what(){return w;}
};
#endif
