#ifndef __TWEAKBAR__H__
#define __TWEAKBAR__H__

#include <array>

#ifdef USE_ANTTWEAKBAR
#include <AntTweakBar.h>
#include <string>
#else
#define TW_CALL
#endif

struct GLFWwindow;

namespace tw
{
    class TweakBar;

    /// Singleton, get current TweakBar (for now, just one bar is supported)
    TweakBar& globalTweakBar();

    /// Graphics API enum
#ifdef USE_ANTTWEAKBAR
    typedef TwGraphAPI GraphicAPI;
#else
    enum GraphicAPI
    {
        TW_OPENGL,
        TW_OPENGL_CORE,
        TW_DIRECT3D9,
        TW_DIRECT3D10,
        TW_DIRECT3D11
    };
#endif
    /// This function initializes the AntTweakBar library. It must be called once at the beginning of the program, just after graphic mode is initialized.
    void init(GraphicAPI api, void* device = nullptr);
    /// When program finishes, terminate must be called, just before graphic mode is terminated.
    void terminate();
    /// draws all tweak bars
    void draw();
    /// call, if the main windows resized
    void mainWindowResized(size_t width, size_t height);
    /// his function defines optional parameters for tweak bars and variables
    int define(const char *def);

    /// TW Type enum
#ifdef USE_ANTTWEAKBAR
    // !!! does not support TW_TYPE_CSSTRING !!!
    typedef TwType Type;
#else
    enum Type
    {
        TW_TYPE_BOOLCPP,
        TW_TYPE_BOOL8,
        TW_TYPE_BOOL16,
        TW_TYPE_BOOL32,
        TW_TYPE_CHAR,
        TW_TYPE_INT8,
        TW_TYPE_UINT8,
        TW_TYPE_INT16,
        TW_TYPE_UINT16,
        TW_TYPE_INT32,
        TW_TYPE_UINT32,
        TW_TYPE_FLOAT,
        TW_TYPE_DOUBLE,
        TW_TYPE_COLOR32,
        TW_TYPE_COLOR3F,
        TW_TYPE_COLOR4F,
        //TW_TYPE_CSSTRING(maxsize),
        TW_TYPE_CDSTRING,
        TW_TYPE_STDSTRING,
        TW_TYPE_QUAT4F,
        TW_TYPE_QUAT4D,
        TW_TYPE_DIR3F,
        TW_TYPE_DIR3D,
    };
    //!!!TW_TYPE_CSSTRING is NOT DEFINED, so don't use it!!!!
#endif
    /// !!!NOT SUPPORTED!!!
#undef TW_TYPE_CSSTRING


    /// callback types
#ifdef USE_ANTTWEAKBAR
    typedef TwSetVarCallback SetVarCallback;
    typedef TwGetVarCallback GetVarCallback;
    typedef TwButtonCallback ButtonCallback;
#else
    typedef void (* SetVarCallback)(const void *value, void *clientData);
    typedef void (* GetVarCallback)(void *value, void *clientData);
    typedef void (* ButtonCallback)(void *clientData);
#endif

    namespace GLFW // Version 3
    {
        /// returns 1, if event was handled by the tweakbar, otherwise 0
        int eventMouseButton(GLFWwindow*, int button, int action, int bits);
        int eventKey(GLFWwindow *, int key, int scancode, int action, int mods);
        int eventChar(GLFWwindow*, unsigned int codepoint);
        int mouseMotion(GLFWwindow*, double x, double y);
        int mouseWheel(int pos);// todo
    }

#ifdef USE_ANTTWEAKBAR
    typedef TwParamValueType ParamValueType;
#else
    enum ParamValueType
    {
        TW_PARAM_INT32,
        TW_PARAM_FLOAT,
        TW_PARAM_DOUBLE,
        TW_PARAM_CSTRING
    };
#endif
}

//////////////// IMPLEMENTATION ///////////////////////

//#include "TweakBar.impl"

//class declaration
class tw::TweakBar
{
#ifdef USE_ANTTWEAKBAR
    TwBar *m_Bar;
    std::string m_name;
#endif
    TweakBar(const char* name);
    ~TweakBar();
    friend tw::TweakBar& tw::globalTweakBar();

public:
    /// get the name of the tweakBar
    std::string name() const;

    int addVarRW(const char *name, tw::Type type, void *var, const char *def);
    int addVarRO(const char *name, tw::Type type, const void *var, const char *def);
    int addVarCB(const char *name, tw::Type type, tw::SetVarCallback setCallback, tw::GetVarCallback getCallback, void *clientData, const char *def);

    int addButton(const char *name, tw::ButtonCallback callback, void *clientData, const char *def);
    int addSeparator(const char* name, const char* def);

    int removeVar(const char* name);
    int removeAllVar();

    int getParam(const char *varName, const char *paramName, tw::ParamValueType paramValueType, size_t outValueMaxCount, void *outValues);
    int getParam(const char *varName, const char *paramName, size_t outValueMaxCount, int32_t& outValues);
    int getParam(const char *varName, const char *paramName, size_t outValueMaxCount, float& outValues);
    int getParam(const char *varName, const char *paramName, size_t outValueMaxCount, double& outValues);
    int getParam(const char *varName, const char *paramName, size_t outValueMaxCount, char* outValues);

    int setParam(const char *varName, const char *paramName, tw::ParamValueType paramValueType, unsigned int inValueCount, const void *inValues);
    int setParam(const char *varName, const char *paramName, unsigned int inValueCount, const int32_t& inValues);
    int setParam(const char *varName, const char *paramName, unsigned int inValueCount, const float& inValues);
    int setParam(const char *varName, const char *paramName, unsigned int inValueCount, const double& inValues);
    int setParam(const char *varName, const char *paramName, unsigned int inValueCount, const char* inValues);

    std::string to_string(bool _v)
    {
        static std::array<std::string,2> tf = { "false", "true" };
        return tf[static_cast<size_t>(_v)];
    }

    void setVisible(bool _v)
    {
#ifdef USE_ANTTWEAKBAR
        std::string param = m_name;
        param += " visible=" + to_string(_v);
        tw::define(param.c_str());
#endif
    }

    void setMinimized(bool _v)
    {
#ifdef USE_ANTTWEAKBAR
        std::string param = m_name;
        param += " iconified=" + to_string(_v);
        tw::define(param.c_str());
#endif
    }

    void setIconified(bool _v)
    {
        setMinimized(true);
    }
};

//class definition

inline int tw::TweakBar::setParam(const char *varName, const char *paramName, tw::ParamValueType paramValueType, unsigned int inValueCount, const void *inValues)
{
#ifdef USE_ANTTWEAKBAR
    return TwSetParam(m_Bar, varName, paramName, paramValueType, inValueCount, inValues);
#endif
    return 1;
}

inline int tw::TweakBar::setParam(const char *varName, const char *paramName, unsigned int inValueCount, const int32_t &inValues)
{
#ifdef USE_ANTTWEAKBAR
    return TwSetParam(m_Bar, varName, paramName, TW_PARAM_INT32, inValueCount, &inValues);
#endif
    return 1;
}

inline int tw::TweakBar::setParam(const char *varName, const char *paramName, unsigned int inValueCount, const float &inValues)
{
#ifdef USE_ANTTWEAKBAR
    return TwSetParam(m_Bar, varName, paramName, TW_PARAM_FLOAT, inValueCount, &inValues);
#endif
    return 1;
}

inline int tw::TweakBar::setParam(const char *varName, const char *paramName, unsigned int inValueCount, const double &inValues)
{
#ifdef USE_ANTTWEAKBAR
    return TwSetParam(m_Bar, varName, paramName, TW_PARAM_DOUBLE, inValueCount, &inValues);
#endif
    return 1;
}

inline int tw::TweakBar::setParam(const char *varName, const char *paramName, unsigned int inValueCount, const char* inValues)
{
#ifdef USE_ANTTWEAKBAR
    return TwSetParam(m_Bar, varName, paramName, TW_PARAM_CSTRING, inValueCount, &inValues);
#endif
    return 1;
}

inline int tw::TweakBar::getParam(const char *varName, const char *paramName, tw::ParamValueType paramValueType, size_t outValueMaxCount, void *outValues)
{
#ifdef USE_ANTTWEAKBAR
    return TwGetParam(m_Bar, varName, paramName, paramValueType, static_cast<int>(outValueMaxCount), outValues);
#endif
    return static_cast<int>(outValueMaxCount);
}
inline int tw::TweakBar::getParam(const char *varName, const char *paramName, size_t outValueMaxCount, int32_t &outValues)
{
#ifdef USE_ANTTWEAKBAR
    return TwGetParam(m_Bar, varName, paramName, TW_PARAM_INT32, static_cast<int>(outValueMaxCount), &outValues);
#endif
    return static_cast<int>(outValueMaxCount);
}
inline int tw::TweakBar::getParam(const char *varName, const char *paramName, size_t outValueMaxCount, float &outValues)
{
#ifdef USE_ANTTWEAKBAR
    return TwGetParam(m_Bar, varName, paramName, TW_PARAM_FLOAT, static_cast<int>(outValueMaxCount), &outValues);
#endif
    return static_cast<int>(outValueMaxCount);
}
inline int tw::TweakBar::getParam(const char *varName, const char *paramName, size_t outValueMaxCount, double &outValues)
{
#ifdef USE_ANTTWEAKBAR
    return TwGetParam(m_Bar, varName, paramName, TW_PARAM_DOUBLE, static_cast<int>(outValueMaxCount), &outValues);
#endif
    return static_cast<int>(outValueMaxCount);
}
inline int tw::TweakBar::getParam(const char *varName, const char *paramName, size_t outValueMaxCount, char *outValues)
{
#ifdef USE_ANTTWEAKBAR
    return TwGetParam(m_Bar, varName, paramName, TW_PARAM_CSTRING, static_cast<int>(outValueMaxCount), outValues);
#endif
    return static_cast<int>(outValueMaxCount);
}

inline std::string tw::TweakBar::name() const
{
#ifdef USE_ANTTWEAKBAR
    return m_name;
#endif
    return std::string();
}

inline int tw::TweakBar::addVarRW(const char *name, tw::Type type, void *var, const char *def)
{
#ifdef USE_ANTTWEAKBAR
    return TwAddVarRW(m_Bar, name, type, var, def);
#endif
    return 1;
}
inline int tw::TweakBar::addVarRO(const char *name, tw::Type type, const void *var, const char *def)
{
#ifdef USE_ANTTWEAKBAR
    return TwAddVarRO(m_Bar, name, type, var, def);
#endif
    return 1;
}
inline int tw::TweakBar::addButton(const char *name, tw::ButtonCallback callback, void *clientData, const char *def)
{
#ifdef USE_ANTTWEAKBAR
    return TwAddButton(m_Bar, name, callback, clientData, def);
#endif
    return 1;
}
inline int tw::TweakBar::addVarCB(const char *name, tw::Type type, tw::SetVarCallback setCallback, tw::GetVarCallback getCallback, void *clientData, const char *def)
{
#ifdef USE_ANTTWEAKBAR
    return TwAddVarCB(m_Bar, name, type, setCallback, getCallback, clientData, def);
#endif
    return 1;
}

inline int tw::TweakBar::addSeparator(const char* name, const char* def)
{
#ifdef USE_ANTTWEAKBAR
    return TwAddSeparator(m_Bar, name, def);
#endif
    return 1;
}
inline int tw::TweakBar::removeVar(const char* name)
{
#ifdef USE_ANTTWEAKBAR
    return TwRemoveVar(m_Bar, name);
#endif
    return 1;
}
inline int tw::TweakBar::removeAllVar()
{
#ifdef USE_ANTTWEAKBAR
    return TwRemoveAllVars(m_Bar);
#endif
    return 1;
}

inline tw::TweakBar::TweakBar(const char* name)
#ifdef USE_ANTTWEAKBAR
:m_Bar(TwNewBar(name)),
m_name(name)
#endif
{
#ifdef USE_ANTTWEAKBAR
#endif
}

inline tw::TweakBar::~TweakBar()
{
#ifdef USE_ANTTWEAKBAR
#endif
}

/////////// global functions ////////////////

inline int tw::define(const char *def)
{
#ifdef USE_ANTTWEAKBAR
    return TwDefine(def);
#else
    return 1;
#endif
}

inline tw::TweakBar& tw::globalTweakBar()
{
    static tw::TweakBar result("Graphics");
    return result;
}

inline void tw::draw()
{
#ifdef USE_ANTTWEAKBAR
    TwDraw();
#endif
}

inline void tw::mainWindowResized(size_t width, size_t height)
{
#ifdef USE_ANTTWEAKBAR
    TwWindowSize(static_cast<int>(width), static_cast<int>(height));
#endif
}

inline void tw::init(tw::GraphicAPI api, void* device)
{
#ifdef USE_ANTTWEAKBAR
    TwInit(api, device);
#endif    
}

inline void tw::terminate()
{
#ifdef USE_ANTTWEAKBAR
    TwTerminate();
#endif    
}


///////// callbacks /////////

//// GLFW ////

inline int tw::GLFW::eventMouseButton(GLFWwindow*, int button, int action, int)
{
#ifdef USE_ANTTWEAKBAR
    return TwEventMouseButtonGLFW(button, action);
#endif
    return 0;
}
inline int tw::GLFW::eventKey(GLFWwindow *, int key, int, int action, int)
{
#ifdef USE_ANTTWEAKBAR
    return TwEventKeyGLFW(key, action);
#endif
    return 0;
}
inline int tw::GLFW::eventChar(GLFWwindow*, unsigned int codepoint)
{
#ifdef USE_ANTTWEAKBAR
    return TwEventCharGLFW(codepoint, 1);
#endif
    return 0;
}
inline int tw::GLFW::mouseMotion(GLFWwindow*, double x, double y)
{
#ifdef USE_ANTTWEAKBAR
    return TwEventMousePosGLFW(static_cast<int>(x), static_cast<int>(y));
#endif
    return 0;
}
inline int tw::GLFW::mouseWheel(int pos)
{
#ifdef USE_ANTTWEAKBAR
    return TwEventMouseWheelGLFW(pos);
#endif
    return 0;
}

#endif //__TWEAKBAR__H__