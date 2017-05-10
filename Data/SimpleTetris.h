/*quadlayouts with position top middle

field
currentQuad* 
currentRenderer* 
1 timer each tick, translate current Quad (if allowed) otherwise, add another quad
1 control callbacker (register callbacks) (functions provided by an interface)
*/

#pragma once

#include <memory>
#include <thread>

#include "../Renderer/FieldGLBuffer.h"
#include "QuadEntity.h"
#include "Field.h"
#include "RandomGenerator.h"

class GLRenderer;
class QuadEntity;

class SimpleTetris
{
    Field m_field;
    FieldGLBuffer m_fieldBuffer;
    RandomGenerator m_generator;
    std::shared_ptr<QuadEntity> m_currentEntity;
    GLRenderer* const m_renderer;

    std::thread m_timerThread;
    bool m_killThread,m_stopTimer;
public:
    SimpleTetris(GLRenderer* renderer);
    ~SimpleTetris();

    void runGame();
    Field& getField(){return m_field;}
    const Field& getField()const{return m_field;}

    /// returns id of selected quad. -1, if no quad was selected. params are the 2d window position
    size_t getQuadID(float2 windowPos);

    std::vector< std::shared_ptr<QuadEntity> > getActiveQuads(){return std::vector< std::shared_ptr<QuadEntity> >(1,m_currentEntity);}
    void translateCurrent(const float3& translation);

    bool isPaused() const { return m_stopTimer; }
    void pause(){ m_stopTimer = true; }
    void reset(){ m_currentEntity.reset(); m_field.clear(); }
    void start(){ m_stopTimer = false; }

};
