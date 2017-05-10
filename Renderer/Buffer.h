#pragma once
#include <vector>
#include <map>
#include "GL/GLHeader.h"


class Buffer
{
    // stores the data
    std::vector<float> m_Data;
    // stores as key the chunksize of free data and as values the memoryoffset
    std::multimap<size_t,size_t> m_sizeMemoryMap;
    // stores as key the start and the end of a free chunk and as values the size of the free chunk
    std::map<size_t,size_t> m_endMemorySizeMap;


    GLuint m_vbo;
    mutable bool m_dirty;

protected:
    void resetDirty() const { m_dirty = false; }
public:
    Buffer();
    ~Buffer();
    void updatePartial(const float* begin, const float* end)const;
    bool dirty()const{ return m_dirty && !m_Data.empty(); }

    //gl
    size_t size()const {return m_Data.size();}

    /// adds data to the buffer and get a resulting offset NOT valid float ptr. use it only with Buffer::data
    float* add(const float* begin,const float* end);
    void erase(const float* begin,const float* end);

    /// returns data from the given offset
    float* data(const float* offset) const;
};


template<size_t BufferType>
class BasicBuffer : public Buffer
{
    GLuint m_vbo;
public:
    BasicBuffer(const BasicBuffer&) = delete;
    const BasicBuffer& operator= (const BasicBuffer&) = delete;
    BasicBuffer() :
        m_vbo(0)
    {
        glGenBuffers(1, &m_vbo);
    }
    ~BasicBuffer()
    {
        glDeleteBuffers(1, &m_vbo);
    }
    void bind() const
    {
        glBindBuffer(BufferType, m_vbo);
    }
    void update() const
    {
        if (!dirty())
            return;
        glBindBuffer(BufferType, m_vbo);
        glBufferData(BufferType, size()*sizeof(float), data(0), GL_STATIC_DRAW);
        resetDirty();
    }
    GLuint getGLId() const
    {
        return m_vbo;
    }
};

typedef BasicBuffer<GL_ARRAY_BUFFER> ArrayBuffer;
typedef BasicBuffer<GL_TEXTURE_BUFFER> TextureBuffer;
