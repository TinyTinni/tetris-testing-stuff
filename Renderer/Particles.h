#pragma once

#include <utility/Vector.hpp>
#include <chrono>

#include "../Renderer/GLRenderer.h"

struct Particle
{
    float3 position;
    float3 direction;
};

class ParticleCannon
{
    float3 m_startPos;
    float3 m_shootDir;
    float3 m_gDir;
    std::chrono::milliseconds m_interval;

    GLRenderer& renderer;

public:
    void start();
    void stop();

};