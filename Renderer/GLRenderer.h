#pragma once

#include <vector>
#include <memory>


#include "GL/GLHeader.h"
#include "Buffer.h"

#include "../Data/QuadEntity.h"
#include "../Data/Field.h"
#include "Font/Font.h"

#include "Screen.h"

#include "FieldGLBuffer.h"


class GLRenderer
{
private:

    //Buffers
    GLuint m_refQuad; //normal Quad
    GLuint m_screenQuad; //screen size quad
    GLuint m_vao; //std vao
        
    TextBox m_testText;

    float m_glowExpand;

    //shaders
    GLuint m_quadProgram;
    GLuint m_lineProgram;
    GLuint m_glowProgram;
    GLuint m_showScreenProgram;
    GLuint m_lightScatteringProgram;

    //redner targets
    GLsizei m_width, m_height;
    GLsizei m_multisample;
    GLuint m_frameBuffer;
    GLuint m_renderTarget;
    GLuint m_depthRenderTarget;   

	// Screen
	Screen m_Screen;
	std::vector< FieldGLBuffer* > m_fieldBuffers;

    float tmpAngle;

public:
    GLRenderer(const unsigned width, const unsigned height);
    ~GLRenderer();
    void render() const;
    void resize(const unsigned width,const unsigned height);


    /// adds a Field to the renderer, field will be rendered on the position given by field itself
    void addField(FieldGLBuffer& b)
    {
        m_fieldBuffers.push_back(&b);
    }
	Screen& getScreen(const FieldGLBuffer& /*b*/) { return m_Screen; }//todo: have multiple screen and each field is dedicated to one screen
	const Screen& getScreen(const FieldGLBuffer& /*b*/) const { return m_Screen; }

private:
    void drawQuadEntitiesInstanced() const;
    void drawLines() const;
    void drawGlow() const;
    
};

