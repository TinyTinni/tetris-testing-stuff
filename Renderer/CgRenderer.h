#pragma once

#ifdef USE_CG

#include "glHeader.h"
#include <Cg/cg.h>
#include <Cg/cgGL.h>

class CgRenderer
{

    CGeffect    m_effect;
    CGcontext   m_cgContext;
    CGparameter m_modelViewProjParameter;
    GLuint      m_vbo;

    void updateVBO();

public:

    void setQuadPositions();

    CgRenderer();
    ~CgRenderer();
    void render();
};

#endif //#ifdef USE_CG