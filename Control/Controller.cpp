#include "Controller.h"
#include <boost/math/special_functions/round.hpp>

#include "../Data/SimpleTetris.h"

float3 Controller::currentCog() const
{
    return m_game.getActiveQuads()[m_id]->cog();
}

void Controller::rotateZ(int direction)
{
    auto quad = m_game.getActiveQuads()[m_id];
    if (!quad)
        return;
    float3 cog = quad->cog();
    cog /= Quad::sideLength();
    cog[0] = boost::math::round(cog[0]);
    cog[1] = boost::math::round(cog[1]);
    cog[2] = boost::math::round(cog[2]);
    cog *= Quad::sideLength();

    float lowerExtension = cog[1] - quad->lowerBound();
    float upperExtension = quad->upperBound() - cog[1];

    if (cog[0] - lowerExtension < m_game.getField().getLeftBound())
        cog[1] -= lowerExtension;
    else if (cog[0] + upperExtension > m_game.getField().getRightBound())
        cog[1] += upperExtension;

    if (m_game.getField().isZRotationValid(*quad, direction, cog))
        quad->rotateZ(direction, cog);
}
void Controller::translateDown()
{
    float3 translation;
    translation.clear();
    translation[0] = translation[2] = 0.f;
    translation[1] = -m_game.getField().fieldSize().y;
    translate(translation);
}
void Controller::translateRight()
{
    float3 translation;
    translation.clear();
    translation[1] = translation[2] = 0.f;
    translation[0] = m_game.getField().fieldSize().x;
    translate(translation);
}
void Controller::translateLeft()
{
    float3 translation;
    translation.clear();
    translation[1] = translation[2] = 0.f;
    translation[0] = -m_game.getField().fieldSize().x;
    translate(translation);
}

void Controller::translate(const float3& translation)
{
    std::shared_ptr<QuadEntity> quad = m_game.getActiveQuads()[m_id];
    if (!quad)
        return;
    if (m_game.getField().isTranslationValid(*quad, translation))
        quad->translate(translation);
}

void Controller::dump()
{
    float3 translation;
    translation[0] = translation[2] = 0.f;
    translation[1] = -m_game.getField().fieldSize().y;
    std::shared_ptr<QuadEntity> quad = m_game.getActiveQuads()[m_id];
    if (!quad)
        return;
    while (m_game.getField().isTranslationValid(*quad, translation))
        quad->translate(translation);
}

size_t Controller::getQuadID(float2 p)
{
    return m_game.getQuadID(p); 
}

bool Controller::isPaused()const 
{ 
    return m_game.isPaused(); 
}
void Controller::pause()
{ 
    m_game.pause(); 
}
void Controller::reset()
{ 
    m_game.reset(); 
}
void Controller::start()
{ 
    m_game.start();
}
