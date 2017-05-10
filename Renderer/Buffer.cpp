#include "Buffer.h"

#include "GL/GLHeader.h"

#include <iostream>
#include <cstdint>

Buffer::Buffer()
    :m_Data(),
    m_dirty(false)
{
}

Buffer::~Buffer()
{
}

void Buffer::updatePartial(const float* begin, const float* end)const
{
    //size_t numElements = reinterpret_cast<const uint8_t*>(end)-reinterpret_cast<const uint8_t*>(begin);

    //glBindBuffer(GL_ARRAY_BUFFER,m_vbo);
    //glBufferSubData(GL_ARRAY_BUFFER,(begin-&m_Data[0])*sizeof(float),numElements,begin);
    m_dirty = true;
}

float* Buffer::add(const float* begin,const float* end)
{
    size_t chunkSize = end-begin;

    // use already allocated memory
    auto iter = m_sizeMemoryMap.lower_bound(chunkSize);
    if (iter != m_sizeMemoryMap.end())
    {
        //chunk was found, copy data
        std::copy(begin,end,&m_Data[0]+iter->second);
        //update pointer
        size_t oldOffset = iter->second;
        size_t oldSize = iter->first;
        m_sizeMemoryMap.erase(iter);
        m_endMemorySizeMap.erase(oldOffset);
        if ((oldSize-chunkSize) == 0)
        {
            m_endMemorySizeMap.erase(oldOffset+oldSize-1);
        }
        else
        {
            size_t newOffset = oldOffset+chunkSize;
            size_t newSize = oldSize-chunkSize;
            // update end
            m_endMemorySizeMap[oldOffset+oldSize-1] = oldSize-chunkSize;
            // update begin
            m_endMemorySizeMap[oldOffset+chunkSize] = oldSize-chunkSize;

            m_sizeMemoryMap.insert(std::make_pair(newSize,newOffset));
        }

        m_dirty = true;
        return reinterpret_cast<float *>(oldOffset);
    }
    // no free chunk was found, so we have to create a new one
    size_t oldSize = m_Data.size();
    size_t newSize = oldSize+chunkSize;
    m_Data.resize(newSize);
    //copy the data from quad group to the internal renderer memory. can only be done, if QuadEntity has continous data (see QuadEntity)
    std::copy(begin,end,&m_Data[oldSize]);
    m_dirty = true;
    return reinterpret_cast<float *>(&m_Data[oldSize]-&m_Data[0]);
}

void Buffer::erase(const float* begin,const float* end)
{
    size_t beginOffset = begin-&m_Data[0];
    size_t endOffset = end-&m_Data[0];
    if (endOffset == m_Data.size())//chunk at the end -> delete
    {
        m_Data.erase(m_Data.begin()+beginOffset,m_Data.end());
        m_dirty = true;
        return;
    }
    size_t chunkSize = end - begin;

    // update the buffer
    std::fill(m_Data.begin()+beginOffset,m_Data.begin()+endOffset,0.f);
    m_dirty = true;

    // tell, where is a free chunk

    //merge with the previous free chunk
    if (beginOffset-1 <= m_Data.size())
    {
        auto frontIter = m_endMemorySizeMap.find(beginOffset-1);
        if (frontIter != m_endMemorySizeMap.end())
        {
            size_t size = frontIter->second;
            size_t offset = frontIter->first-(size-1);

            //remove old chunk
            m_endMemorySizeMap.erase(frontIter);
            auto sizeIter = m_sizeMemoryMap.find(size);
            while(sizeIter->second != offset)
                ++sizeIter;
            m_sizeMemoryMap.erase(sizeIter);

            //update current chunk
            chunkSize += size;
            beginOffset -= size;
        }
    }
    //merge with the next free chunk
    auto backIter = m_endMemorySizeMap.find(beginOffset+chunkSize);
    if (backIter != m_endMemorySizeMap.end())
    {
        size_t size = backIter->second;
        size_t offset = backIter->first;

        //remove old
        m_endMemorySizeMap.erase(backIter);
        auto sizeIter = m_sizeMemoryMap.find(size);
        while(sizeIter->second != offset) 
            ++sizeIter;
        m_sizeMemoryMap.erase(sizeIter);

        //update current
        chunkSize += size;
    }

    //merge with the free chunk at the end
    m_sizeMemoryMap.insert(std::make_pair(chunkSize,beginOffset));
    m_endMemorySizeMap[beginOffset+chunkSize-1] = chunkSize;
    m_endMemorySizeMap[beginOffset] = chunkSize;
}

float* Buffer::data(const float* offset) const
{
    return const_cast<float*>(&m_Data[0])+reinterpret_cast<size_t>(offset);
}
