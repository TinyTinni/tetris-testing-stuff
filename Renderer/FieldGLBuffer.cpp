#include "FieldGLBuffer.h"

#include "Buffer.h"
#include "GLRenderer.h"

#include "../Data/Field.h"

struct FieldGLBuffer::Impl
{
    TextureBuffer m_quadData;
    GLuint m_quadTextureBuffer;
    ArrayBuffer m_lineBuffer;
    float4x4 m_modelMatrix;
    Field& m_field;
    GLRenderer& m_renderer;
    Impl(Field& f, GLRenderer& r) :m_quadData(),
        m_quadTextureBuffer(0),
        m_lineBuffer(),
        m_modelMatrix(matrix::identity<decltype(m_modelMatrix)>()),
        m_field(f),
        m_renderer(r)
    {
        glGenTextures(1, &m_quadTextureBuffer);
    }

    ~Impl()
    {
        glDeleteTextures(1, &m_quadTextureBuffer);
    }

};

FieldGLBuffer::FieldGLBuffer(Field& f, GLRenderer& r):
m_impl(new Impl(f,r))
{
    r.addField(*this);

    // fill up the line buffer
    const float leftX = m_impl->m_field.getX();
    const float rightX = m_impl->m_field.getX() + m_impl->m_field.getWidth();
    const float topY = m_impl->m_field.getY() + m_impl->m_field.getHeight();
    const float bottomY = m_impl->m_field.getY();

    //outlines
    const float halfLineWidth = 0.0125f;
    float lines[48] = {
        //left
        leftX - halfLineWidth, topY + halfLineWidth, -2.f,
        1.0f, 0.0f, 0.0f,
        leftX - halfLineWidth, bottomY - halfLineWidth, -2.f,
        1.f, 0.f, 0.f,

        //right
        rightX + halfLineWidth, topY + halfLineWidth, -2.f,
        1.0f, 0.0f, 0.0f,
        rightX + halfLineWidth, bottomY - halfLineWidth, -2.f,
        1.f, 0.f, 0.f,

        //bottom
        rightX + halfLineWidth + halfLineWidth, bottomY - halfLineWidth, -2.f,
        1.0f, 0.0f, 0.0f,
        leftX - halfLineWidth, bottomY - halfLineWidth, -2.f,
        1.f, 0.f, 0.f,

        //middle part
        -0.5625f, -0.1f, -2.f,
        1.0f, 0.0f, 0.0f,
        -0.5625f, 0.1f, -2.f,
        1.f, 0.f, 0.f,
    };

    std::vector<GLfloat> middleLines(48, 0.f);
    std::copy(lines, lines + 48, &middleLines[0]);
    const float b = 0.3f;
    for (size_t i = 1; i < m_impl->m_field.getCellXCount() + 1; ++i)
    {
        //horizontal
        middleLines.push_back(rightX);
        middleLines.push_back(bottomY + Quad::sideLength()*i);
        middleLines.push_back(-2.0f);

        middleLines.push_back(0.f);
        middleLines.push_back(0.f);
        middleLines.push_back(b);

        middleLines.push_back(leftX);
        middleLines.push_back(bottomY + Quad::sideLength()*i);
        middleLines.push_back(-2.0f);

        middleLines.push_back(0.f);
        middleLines.push_back(0.f);
        middleLines.push_back(b);
    }
    for (size_t i = 0; i < m_impl->m_field.getCellYCount(); ++i)
    {
        //vertical
        middleLines.push_back(leftX + Quad::sideLength()*i);
        middleLines.push_back(bottomY);
        middleLines.push_back(-2.0f);

        middleLines.push_back(0.f);
        middleLines.push_back(0.f);
        middleLines.push_back(b);

        middleLines.push_back(leftX + Quad::sideLength()*i);
        middleLines.push_back(topY);
        middleLines.push_back(-2.0f);

        middleLines.push_back(0.f);
        middleLines.push_back(0.f);
        middleLines.push_back(b);

    }

    m_impl->m_lineBuffer.add(middleLines.data(), middleLines.data() + middleLines.size());
}
FieldGLBuffer::~FieldGLBuffer()
{
    m_impl->m_field.clear();//clear all quad entities first
    delete m_impl;
}

bool FieldGLBuffer::addQuadEntity(std::shared_ptr<QuadEntity> entity)
{
    if (!m_impl->m_field.addQuadEntity(entity))
        return false;
    addQuadEntity(*entity);
    return true;
}

void FieldGLBuffer::bindLineBuffer() const
{
    m_impl->m_lineBuffer.bind();
}

void FieldGLBuffer::updateLineBuffer()
{
    m_impl->m_lineBuffer.update();
}

/// binds and updates the current quad buffer with texture
void FieldGLBuffer::bindQuadBuffer() const
{
    m_impl->m_quadData.bind();
    glBindTexture(GL_TEXTURE_BUFFER, m_impl->m_quadTextureBuffer);
}

size_t FieldGLBuffer::quadSize() const
{
    return m_impl->m_quadData.size();
}

void FieldGLBuffer::updateQuadBuffer()
{
    m_impl->m_quadData.update();
    glBindTexture(GL_TEXTURE_BUFFER, m_impl->m_quadTextureBuffer);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, m_impl->m_quadData.getGLId());
}

void FieldGLBuffer::setModelMatrix(const float4x4& matrix)
{
    m_impl->m_modelMatrix = matrix;
}

const float4x4& FieldGLBuffer::getModelMatrix() const
{
    return m_impl->m_modelMatrix;
}

Quad* FieldGLBuffer::quadDataPtr(Quad* offset)
{
    return reinterpret_cast<Quad*>(m_impl->m_quadData.data(reinterpret_cast<float*>(offset)));
}

void FieldGLBuffer::addQuadEntity(QuadEntity& entity)
{
    float* offset = m_impl->m_quadData.add(&entity[0][0], &entity[entity.size() - 1][Quad::size() - 1] + 1);
    entity.registerBuffer(this, reinterpret_cast<Quad*>(offset));//register renderer and save the index 
    entity.informPositionChanged([this](const QuadEntity& e){this->updateQuads(e, e.begin(), e.end()); });
    m_impl->m_quadData.update();
}

void FieldGLBuffer::updateQuads(const QuadEntity& entity, const Quad* first, const Quad* last)
{
    if (!entity.checkBuffer(this))
        return;
    m_impl->m_quadData.updatePartial(&(*first)[0], reinterpret_cast<const float*>(last));
}
void FieldGLBuffer::deleteQuads(const QuadEntity& entity, const Quad* first, const Quad* last)
{
    if (!entity.checkBuffer(this))
        return;
    m_impl->m_quadData.erase(&(*first)[0], reinterpret_cast<const float*>(last));
}