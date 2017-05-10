#include "GLRenderer.h"
#include "GL/GLShaders.h"
#include "GL/GLErrorHandling.h"
#include "Font/Font.h"

#include <cassert>
#include <boost/math/constants/constants.hpp>

#include <utility/TweakBar.h>
#include <utility/Projection.h>

void TW_CALL setTextCallback(const void *value, void *clientData)
{
    reinterpret_cast<TextBox*>(clientData)->setText(*reinterpret_cast<const std::string*>(value));
}

void TW_CALL getTextCallback(void *value, void *clientData)
{
    *reinterpret_cast<std::string*>(value) = reinterpret_cast<TextBox*>(clientData)->text();
}

GLRenderer::GLRenderer(const unsigned width, const unsigned height) :
    m_screenQuad(0),
    m_vao(0),
    m_quadProgram(0),
    m_glowExpand(0.3f),
    m_lineProgram(0),
    m_glowProgram(0),
    m_width(640),
    m_height(480),
    m_multisample(4),
    m_frameBuffer(0),
    m_renderTarget(0),
    m_depthRenderTarget(0)
{
    glGenBuffers(1, &m_screenQuad);
    float screenQuad[12] =
    {
        -1.f, -1.f, 1.f,
        -1.f, 1.f, 1.f,
        1.f, -1.f, 1.f,
        1.f, 1.f, 1.f,
    };
    glBindBuffer(GL_ARRAY_BUFFER, m_screenQuad);
    glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), &screenQuad[0], GL_STATIC_DRAW);

    registerGLErrorCallbacks();

    //create render target
    glGenFramebuffers(1, &m_frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);

    glGenTextures(1, &m_renderTarget);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_renderTarget);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_multisample, GL_RGBA, m_width, m_height, false);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, m_renderTarget, 0);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

    // Set the list of draw buffers.
    GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

    GLuint m_vao = 0;
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);
    // create programs
    m_quadProgram = glCreateProgram();
    compileLinkProgram<shader::vertex::QuadInstancedShader, shader::fragment::QuadInstancedShader>(m_quadProgram);
    m_lineProgram = glCreateProgram();
    compileLinkProgram<shader::vertex::LineShader,shader::geometry::LineShader, shader::fragment::LineShader>(m_lineProgram);
    m_glowProgram = glCreateProgram();
    compileLinkProgram<shader::vertex::GlowShader,shader::geometry::GlowShader, shader::fragment::GlowShader>(m_glowProgram);
    m_showScreenProgram = glCreateProgram();
    compileLinkProgram<shader::vertex::PresentScreenShader, shader::fragment::PresentScreenShader>(m_showScreenProgram);

    float refQuad[36] =
    {
        -1.0, 1.0, 1.0,
        0.0, 0.0, 0.0,
        1.0, 1.0, 1.0,
        
        -1.0, 1.0, 1.0,
        -1.0, -1.0, 1.0,
        0.0, 0.0, 0.0,

        1.0, 1.0, 1.0,
        0.0, 0.0, 0.0,
        1.0, -1.0, 1.0,
        
        -1.0, -1.0, 1.0,
        1.0, -1.0, 1.0,
        0.0, 0.0, 0.0,
    };

    glGenBuffers(1, &m_refQuad);
    glBindBuffer(GL_ARRAY_BUFFER, m_refQuad);
    glBufferData(GL_ARRAY_BUFFER, 36*sizeof(float), refQuad, GL_STREAM_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // create matrices
    resize(width,height);    

    tw::globalTweakBar().addVarRW("GlowExpand", tw::Type::TW_TYPE_FLOAT, &m_glowExpand, " min=0 max=100 step=0.01");
    tw::globalTweakBar().setMinimized(true);

    //loadFont("Renderer\\Font\\font.fnt");
    tw::globalTweakBar().addVarCB("set_dynamic_font", tw::Type::TW_TYPE_STDSTRING, setTextCallback, getTextCallback, reinterpret_cast<void *>(&m_testText), "label=\"Set Font\"");


    tmpAngle = 0;
    tw::globalTweakBar().addVarRW("Angle", tw::Type::TW_TYPE_FLOAT, &tmpAngle, "min=-3.14 max=3.14 step=0.01");
}

//////////////////////////////////////////////

GLRenderer::~GLRenderer()
{
    glDeleteBuffers(1, &m_screenQuad);

    glDeleteProgram(m_quadProgram);
    glDeleteProgram(m_lineProgram);
    glDeleteProgram(m_glowProgram);
    glDeleteProgram(m_showScreenProgram);
    //glDeleteProgram(m_lightScatteringProgram);

    glDeleteFramebuffers(1, &m_frameBuffer);
    glDeleteTextures(1, &m_renderTarget);
    glDeleteTextures(1, &m_depthRenderTarget);

    glDeleteVertexArrays(1, &m_vao);
}
//////////////////////////////////////////////
void GLRenderer::drawGlow() const
{
    glUseProgram(m_glowProgram);
    glCullFace(GL_FRONT_AND_BACK);
    //set uniforms
    static const GLuint projUniform = glGetUniformLocation(m_glowProgram, "uProjection");
    glUniformMatrix4fv(projUniform, 1, GL_TRUE, &globalProjection().matrix().data()[0]);

    const GLuint uGlowExpand = glGetUniformLocation(m_glowProgram, "uGlowExpansion");
    glUniform1f(uGlowExpand, m_glowExpand);
    

    // enable states
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glEnable(GL_BLEND);
    
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glUniform1f(uGlowExpand, m_glowExpand);
    static const GLuint modelUniform = glGetUniformLocation(m_glowProgram, "uModelMatrix");

    std::for_each(std::begin(m_fieldBuffers), std::end(m_fieldBuffers), [](decltype(*std::begin(m_fieldBuffers)) buffer)
    {
        buffer->bindLineBuffer();
        buffer->updateLineBuffer();
        glUniformMatrix4fv(modelUniform, 1, GL_TRUE, &buffer->getModelMatrix().data()[0]);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);//Position
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));//Color
        
        glDrawArrays(GL_LINES, 0, 6);
    });


    glUniform1f(uGlowExpand, m_glowExpand*0.125f);
    std::for_each(std::begin(m_fieldBuffers), std::end(m_fieldBuffers), [](decltype(*std::begin(m_fieldBuffers)) buffer)
    {
        buffer->bindLineBuffer();
        buffer->updateLineBuffer();
        glUniformMatrix4fv(modelUniform, 1, GL_TRUE, &buffer->getModelMatrix().data()[0]);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);//Position
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));//Color

        glDrawArrays(GL_LINES, 8, 60);
    });
    

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    glDisable(GL_BLEND);
}

//////////////////////////////////////////////
void GLRenderer::drawLines() const
{
    glUseProgram(m_lineProgram);

    //set uniforms
    static const GLuint projUniform = glGetUniformLocation(m_lineProgram, "uProjection");
    glUniformMatrix4fv(projUniform, 1, GL_TRUE, &globalProjection().matrix().data()[0]); 
    static const GLuint modelUniform = glGetUniformLocation(m_lineProgram, "uModelMatrix");

    glEnableVertexAttribArray(0);

    // enable states
    std::for_each(std::begin(m_fieldBuffers), std::end(m_fieldBuffers), [](decltype(*std::begin(m_fieldBuffers)) buffer)
    {
        buffer->bindLineBuffer();
        buffer->updateLineBuffer();
        glUniformMatrix4fv(modelUniform, 1, GL_TRUE, &buffer->getModelMatrix().data()[0]);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);//Position
        glDrawArrays(GL_LINES, 0, 6);
    });

    glDisableVertexAttribArray(0);

}

void GLRenderer::drawQuadEntitiesInstanced() const
{
    glUseProgram(m_quadProgram);

    //set uniforms
    static const GLuint projUniform = glGetUniformLocation(m_quadProgram, "uProjection");
    glUniformMatrix4fv(projUniform, 1, GL_TRUE, &globalProjection().matrix().data()[0]);

    static const GLuint modelUniform = glGetUniformLocation(m_quadProgram, "uModelMatrix");

    static const GLuint renderTargetUniform = glGetUniformLocation(m_quadProgram, "textureBuffer");

    // enable states
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
    glEnable(GL_BLEND);

    //draw buffer
    glActiveTexture(GL_TEXTURE0 + 0);
    glUniform1i(renderTargetUniform, 0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, m_refQuad);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);//Position

    for (const auto& field : m_fieldBuffers)
    {
        if (field->quadSize() > 0)
        {
            field->updateQuadBuffer();
            glUniformMatrix4fv(modelUniform, 1, GL_TRUE, &field->getModelMatrix().data()[0]);
            glDrawArraysInstanced(GL_TRIANGLES, 0, 12, static_cast<GLsizei>(field->quadSize()*1.f / Quad::size()));
        }
    }

    glDisableVertexAttribArray(0);

    // disable states
    glDisable(GL_BLEND);
}

//////////////////////////////////////////////
void GLRenderer::render() const
{
    // set global functions 

    float4x4 matrix = matrix::rotateY(matrix::identity<float4x4>(),tmpAngle);
    m_fieldBuffers[0]->setModelMatrix(matrix);

    glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    drawQuadEntitiesInstanced();

    drawLines();   

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_frameBuffer);
    glDrawBuffer(GL_BACK);
    glBlitFramebuffer(0, 0, m_width, m_height, 0, 0, m_width, m_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    drawGlow();

    //drawFonts(globalProjection().width(),globalProjection().height());

    //draw light scattering

}
//////////////////////////////////////////////
void GLRenderer::resize(const unsigned width,const unsigned height)
{
    m_width = width;
    m_height = height;
    glViewport(0,0,m_width,m_height);
    globalProjection().setPerspective(60.0f, m_width, m_height, 1.f, 5.0f);

    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_renderTarget);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_multisample, GL_RGBA, (GLsizei)m_width, (GLsizei)m_height, false);
}
