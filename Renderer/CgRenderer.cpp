#ifdef USE_CG

#include "ErrorHandling.h"
#include "glHeader.h"

#include "CgRenderer.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <glm/gtx/string_cast.hpp>
#include <iostream>

enum class GenericVertexInputIndices
{
    POSITION        = 0,
    BLENDWEIGHT     = 1,
    NORMAL          = 2,
    DIFFUSE         = 3,    COLOR0      = 3,
    SPECULAR        = 4,    COLOR1      = 4,
    TESSFACTOR      = 5,    FOGCOORD    = 5,
    PSIZE           = 6,
    BLENDINDICES    = 7,
    TEXCOORD0       = 8,
    TEXCOORD1       = 9,
    TEXCOORD2       = 10,
    TEXCOORD3       = 11,
    TEXCOORD4       = 12,
    TEXCOORD5       = 13,
    TEXCOORD6       = 14,   TANGENT     = 14,
    TEXCOORD7       = 15,   BINORMAL    = 15,
};

///////////////////////////////////////////
CgRenderer::CgRenderer()
:m_cgContext(0), m_effect(0)
{
    registerGLErrorCallbacks();    

#ifdef _DEBUG
    cgGLSetDebugMode(CG_TRUE);
#else
    cgGLSetDebugMode(CG_FALSE);
#endif   

    m_cgContext = cgCreateContext();
    registerCgContext(m_cgContext);

    cgSetParameterSettingMode(m_cgContext, CG_IMMEDIATE_PARAMETER_SETTING );

    cgGLRegisterStates(m_cgContext);
    m_effect = cgCreateEffectFromFile( m_cgContext, "Shaders\\test.cgfx", NULL );

    m_modelViewProjParameter = cgGetNamedEffectParameter(m_effect, "modelViewProjectionMatrix");

    glGenBuffers(1,&m_vbo);
    updateVBO();

}
///////////////////////////////////////////
CgRenderer::~CgRenderer()
{
    cgDestroyEffect(m_effect);
    cgDestroyContext(m_cgContext);
}
///////////////////////////////////////////
void CgRenderer::render()
{

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 projectionMatrix = glm::perspective(60.0f, 640.f / 480.f, 0.1f, 100.f);

    cgGLSetMatrixParameterfr(m_modelViewProjParameter,&projectionMatrix[0][0]);    

    CGpass pass = cgGetFirstPass( cgGetFirstTechnique(m_effect) );
    //while( pass ) 
    {
        //cgSetPassState( pass );

        CGprofile vprofile = cgGLGetLatestProfile( CG_GL_VERTEX );
        cgGLSetOptimalOptions(vprofile);

        CGprogram vProgram = cgCreateProgramFromFile(m_cgContext,CG_SOURCE,"Shaders\\test.cg",vprofile,"vmain",0);

        cgGLLoadProgram(vProgram);
        
        cgGLBindProgram(vProgram);

        cgGLEnableProfile(vprofile);

        const float PositionData[] =
        {
            -0.8f, 0.8f,
            0.8f, 0.8f,
            0.0f, -0.8f,
        };

        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        CGparameter param = cgGetNamedParameter(vProgram, "position");
        cgGLEnableClientState(param);
        cgGLSetParameterPointer(param, 2, GL_FLOAT, 0, PositionData);
        
 
        //glEnableVertexAttribArray(m_vbo);
        //glVertexAttribPointer((GLuint)GenericVertexInputIndices::POSITION,2,GL_FLOAT,GL_FALSE,0,0);
        cgUpdateProgramParameters(vProgram);
        glDrawArrays(GL_TRIANGLES,0,1);

        cgGLDisableProfile(vprofile);

        //cgGLDisableClientState(cgGetNamedEffectParameter(m_effect, "position"));

        //cgResetPassState( pass );
        //pass = cgGetNextPass( pass );
    }
   
}

///////////////////////////////////////////
void CgRenderer::setQuadPositions()
{
}

void CgRenderer::updateVBO()
{
    const float PositionData[] =
    {
        -0.08f, 0.08f,
        0.08f, 0.08f,
        0.0f, -0.08f,
    };

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, 6*sizeof(float), PositionData, GL_STATIC_DRAW);

}


#endif //#ifdef USE_CG