#include "QuadEntity.h"

#include <cassert>
#include <functional>
#include <limits>
#include <utility/ArrayAccessor.hpp>

#include "../Renderer/FieldGLBuffer.h"

void QuadEntity::translate(const float x,const float y,const float z)
{
    for (Quad* iter = begin(); iter != end(); ++iter)
    {
        (*iter)[0] += x;
        (*iter)[1] += y;
        (*iter)[2] += z;
    }
   m_positionChangedSignal(*this);
}

Quad* QuadEntity::m_quadsProxy()const
{
    return (m_buffer) ?m_buffer->quadDataPtr(m_quadsRawPtr)  : m_quadsRawPtr;
}

void QuadEntity::rotateZ(const int direction)
{
    rotateZ(direction,cog());
}  

void QuadEntity::rotateZ(const int direction,const float3& origin)
{
    assert( direction != 0 );
    int sig = (direction < 0) ? 1 : -1;
    for (Quad* iter = begin(); iter != end(); ++iter)
    {
        float xPos = iter->position()[0];
        float3 distance = iter->position()-origin;
        float xOrigin = origin[0];
        float xDistance = distance[1];
        float x = origin[0]+sig*distance[1];
        (*iter)[0] = origin[0]+sig*distance[1];
        (*iter)[1] = origin[1]-sig*distance[0];
    }
    m_positionChangedSignal(*this);
}  

void QuadEntity::clear()
{
    if (m_buffer)
    {
        m_buffer->deleteQuads(*this,begin(),end());
        m_buffer = 0;
    }
    else
    {
        delete[] m_quadsRawPtr;
    }
    m_size = 0;
}

void QuadEntity::registerBuffer(FieldGLBuffer* buffer, Quad *newBufferPos)
{
    if(!m_buffer && m_quadsRawPtr) 
        delete[] m_quadsRawPtr;
    m_buffer = buffer;
    m_quadsRawPtr = newBufferPos;
}

bool QuadEntity::checkBuffer(const FieldGLBuffer* buffer) const
{
    return (buffer == m_buffer);
}

float3 QuadEntity::cog() const
{
    float lo = lowerBound(), u = upperBound(), r = rightBound(), le = leftBound();
    float3 result;
    result[0] = 0.5f*(r+le);
    result[1] = 0.5f*(u+lo);
    result[2] = (*beginPosition())[2];
    return result;
}

float QuadEntity::lowerBound() const
{
    return (*std::min_element(beginPosition(),endPosition(),ArrayAccessor<float3,1,std::less<float> >()))[1]; // TODO: cache the value and translate/rotate it
}
float QuadEntity::upperBound() const
{
    return (*std::max_element(beginPosition(),endPosition(),ArrayAccessor<float3,1,std::less<float> >()))[1]; // TODO: cache the value and translate/rotate it
}
float QuadEntity::rightBound() const
{
    return (*std::max_element(beginPosition(),endPosition(),ArrayAccessor<float3,0,std::less<float> >()))[0]; // TODO: cache the value and translate/rotate it
}
float QuadEntity::leftBound() const
{
    return (*std::min_element(beginPosition(),endPosition(),ArrayAccessor<float3,0,std::less<float> >()))[0]; // TODO: cache the value and translate/rotate it
}

float QuadEntity::lowerBound(float x, unsigned checkDigits) const
{
    assert(sqrt(std::numeric_limits<unsigned>::max()) >= checkDigits);
    unsigned multi = static_cast<unsigned>(std::pow(10,checkDigits));
    int cmpValue = static_cast<int>(multi*x);
    auto min = endPosition();
    auto checkX = [x,cmpValue,multi](const float3& pos)
    {
        return (static_cast<int>(multi*pos[0]) == cmpValue);
    };
    for(auto iter = beginPosition(); iter != endPosition() && !checkX(*min); ++iter)
    {
        if (checkX(*iter))
            min = iter;
    }
    if (min == endPosition())
        return std::numeric_limits<float>::quiet_NaN();

    for (auto iter = min; iter != endPosition(); )
    {
        ++iter; // jump over min
         if ((*iter)[1] < (*min)[1] &&  checkX(*iter))
            min = iter;
    }
    // cannot use, because the defined function is not well defined: (a < b) = false and (b < a) = false, are possible solutions
    //auto iter = std::min_element(beginPosition(),endPosition(),[x, multi](const float3& lhs, const float3 rhs)
    //{
    //    return ((lhs[1] < rhs[1]) || (int(multi*rhs[0]) != int(multi*x)));
    //});
    
    return (*min)[1];
}


void QuadEntity::erase(const size_t first,const size_t numElements)
{
    assert(first < m_size);
    assert(numElements < m_size);
    assert(first+numElements <= m_size);
    if (m_buffer)
        m_buffer->deleteQuads(*this,begin()+first,begin()+numElements);
    else
        assert(false);// TODO: implement
}

void QuadEntity::erase(const Quad* quad)
{
    size_t n = 0;
    for (; n < m_size; ++n)
        if ( &(*this)[n] == quad)
            break;
    if (n == m_size)
        return;
    erase(n,1);
}
