#include "SimpleTetris.h"
#include "../Renderer/GLRenderer.h"

#include <utility/Projection.h>

#include <functional>
#include <mutex>

#include <utility/TweakBar.h>

std::mutex translationMutex;

SimpleTetris::SimpleTetris(GLRenderer* renderer):
m_field(20,10,0.0f,0.0f,Quad::sideLength()),
m_fieldBuffer(m_field,*renderer),
m_generator(),
m_currentEntity(0),
m_renderer(renderer),
m_killThread(false),m_stopTimer(true)
{
    m_timerThread = std::thread([this]()
    {
        float3 tr;
        tr[0] = 0.f;
        tr[1] = -Quad::sideLength();
        tr[2] = 0.f;
        while(!m_killThread)
        {
            while(!m_stopTimer)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(500));

                this->translateCurrent(tr);
            }
        }
    });

    tw::globalTweakBar().addVarRW("DisableGame", tw::Type::TW_TYPE_BOOLCPP, &m_stopTimer, "label='Disable Game'");    
}

SimpleTetris::~SimpleTetris()
{
    m_killThread = true;
    m_stopTimer = true;
    m_timerThread.join();
    reset();
}

void SimpleTetris::runGame()
{
   
    //while(!m_stopTimer)
    {
        if (!m_currentEntity)
        {
            std::lock_guard<std::mutex> lock(translationMutex);
            m_currentEntity = m_generator.generateQuad();
            if (!m_fieldBuffer.addQuadEntity(m_currentEntity))
            {
                m_currentEntity.reset();
                return;
            }
        }
    }
}

void SimpleTetris::translateCurrent(const float3& translation)
{
    if (!m_currentEntity)
        return;

    if (m_stopTimer)
        return;

    if (m_field.isTranslationValid(*m_currentEntity,translation))
    {
        m_currentEntity->translate(translation);
    }
    else
    {
		std::lock_guard<std::mutex> lock(translationMutex);
        //m_stopTimer = true;
        m_field.lockEntity(*m_currentEntity);
        float lw = m_currentEntity->lowerBound();
        for (float row = m_currentEntity->lowerBound(); row <= m_currentEntity->upperBound(); row += Quad::sideLength())
            if (m_field.rowFull(row))
            {
                m_field.eraseQuadsInRow(row);
                m_field.rotateDown(row);
                row -= Quad::sideLength();
            }               
        //m_field.swapRow(lw,lw+Quad::sideLength());
        m_currentEntity.reset();
    }
}

/// returns true, if given window Pos lies inside the Quadentity. z coordinate will be ignored!
bool isInEntity(const QuadEntity& entity, const FieldGLBuffer& buffer, float3 position)
{
    float4x4 m = buffer.getModelMatrix();
    float3 cog = entity.cog();
    float4 cog4;
    
    std::copy(cog.begin(), cog.end(), cog4.begin());
    cog4[3] = 1;
    cog4 = boost::numeric::ublas::prod(m, cog4);
    std::copy(cog4.begin(), cog4.end() - 1, cog.begin());

    cog -= position;

    float length = boost::numeric::ublas::norm_2(cog);

    return length <= Quad::sideLength()*2.0f;
}

size_t SimpleTetris::getQuadID(float2 windowPos)
{

    float3 pos3d = globalProjection().unproject(windowPos);
    //transform 2d pos to ray
    if (isInEntity(*m_currentEntity, m_fieldBuffer, pos3d))
    {
        return 1;
    }

    return 0;
    //first, get the field, which has the position

    //get current of this field, is pos includes the entity -> return
}