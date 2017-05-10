#include "RandomGenerator.h"

#include <cstdlib>     /* srand, rand */
#include <ctime>  



RandomGenerator::RandomGenerator()
:m_layouts()
{
    srand(static_cast<unsigned int>(time(NULL)));

    Layout<4> layout;
    layout.set(1, 0);
    layout.set(0, 1);
    layout.set(1, 1);
    layout.set(2, 1);
    m_layouts.push_back(layout);
    layout.reset();

    layout.set(0, 0);
    layout.set(1, 0);
    layout.set(2, 0);
    layout.set(3, 0);
    m_layouts.push_back(layout);
    layout.reset();

    layout.set(0, 0);
    layout.set(1, 0);
    layout.set(0, 1);
    layout.set(1, 1);    
    m_layouts.push_back(layout);
    layout.reset();

    layout.set(0, 0);
    layout.set(1, 0);
    layout.set(2, 0);
    layout.set(2, 1);
    m_layouts.push_back(layout);
    layout.reset();

    layout.set(0, 0);
    layout.set(1, 0);
    layout.set(1, 1);
    layout.set(2, 1);
    m_layouts.push_back(layout);
    layout.reset();

    layout.set(0, 1);
    layout.set(1, 1);
    layout.set(1, 0);
    layout.set(2, 0);
    m_layouts.push_back(layout);
    layout.reset();

}
RandomGenerator::~RandomGenerator()
{

}

std::shared_ptr<QuadEntity> RandomGenerator::generateQuad()
{
    std::shared_ptr<QuadEntity> result = std::make_shared<QuadEntity>(4);
    m_layouts[rand()%m_layouts.size()].setLayout(*result);
    for (int i = 0; i < 4; ++i)
    {
        float r = static_cast<float>(rand() % 2);
        float g = 0.f, b = 0.f;
        if (!r)
        {
            g = static_cast<float>(rand() % 2);
            if (!g)
                b = 1.f;
        }
        (*result)[i].setColor(r, g, b);
    }
    return result;
}