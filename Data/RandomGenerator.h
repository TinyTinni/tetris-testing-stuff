#pragma once

#include <memory>
#include <vector>
#include <algorithm>
#include "QuadEntity.h"

#include <array>
template<size_t n>
class Layout
{
    std::array<bool, n*n> m_data;
    size_t twoToOne(size_t x, size_t y)
    {
        return x*n + y;
    }
    float2 getCoords(size_t index)
    {
        int x = static_cast<int>((index / n)) - (n / 2)+1;
        int y = static_cast<int>(index % n);
        float2 result;
        result[0] = x * Quad::sideLength();
        result[1] = (9 - y) * Quad::sideLength();
        return result;
    }
public:
    Layout() :m_data({ false }){}
    void reset()
    {
        std::fill(m_data.begin(), m_data.end(), false);
    }
    void set(size_t x, size_t y, bool v = true)
    {
        m_data[twoToOne(x, y)] = v;
    }
    bool get(size_t x, size_t y)
    {
        return m_data[twoToOne(x, y)];
    }
    void setLayout(QuadEntity& entity)
    {
        //assert(entity.size() <= n);
        size_t entityIndex = 0;
        for (size_t i = 0; i < n*n; ++i)
        {
            if (m_data[i])
            {
                float2 coords = getCoords(i);
                entity[entityIndex++].setPosition(coords[0], coords[1], -2.f);
            }
        }
    }
};

class RandomGenerator
{
    std::vector<Layout<4> > m_layouts;
public:
    RandomGenerator();
    ~RandomGenerator();
    std::shared_ptr<QuadEntity> generateQuad(/*centerPos*/);
};