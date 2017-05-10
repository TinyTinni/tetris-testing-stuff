#pragma once

#include <memory>

#include <utility/Vector.hpp>
#include "../Data/QuadEntity.h"


class Field;
class GLRenderer;

class FieldGLBuffer
{
    struct Impl;
    Impl* m_impl;

public:

    FieldGLBuffer(const FieldGLBuffer&) = delete;
    const FieldGLBuffer& operator=(const FieldGLBuffer&) = delete;

    FieldGLBuffer(Field& f, GLRenderer& r);
    ~FieldGLBuffer();

    /// add entity to Renderer and to the physical field
    bool addQuadEntity(std::shared_ptr<QuadEntity> entity);

    /// bind the line buffer
    void bindLineBuffer() const;

    void updateLineBuffer();

    /// binds and updates the current quad buffer with texture
    void bindQuadBuffer() const;

    void updateQuadBuffer();

    size_t quadSize()const;

    void setModelMatrix(const float4x4& matrix);

    const float4x4& getModelMatrix() const;

private:
    friend QuadEntity;
    /** update quads in the quadentity
    * @param entity Entity
    * @param first first Quad that should be updated
    * @param last first Quad, that wouldn't be updated
    */
    /// Quad Handling
    /// returns dataptr with offset given by addQuadEntitiy
    Quad* quadDataPtr(Quad* offset);
    void addQuadEntity(QuadEntity& entity);
    void updateQuads(const QuadEntity& entity, const Quad* first, const Quad* last);
    void deleteQuads(const QuadEntity& entity, const Quad* first, const Quad* last);
    
};