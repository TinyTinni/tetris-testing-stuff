#pragma once
#include <array>
#include <vector>
#include <map>
#include <tuple>
#include <algorithm>

#include <boost/multi_array.hpp>
#include <boost/math/special_functions/round.hpp>

#include <utility/Vector.hpp>

class QuadEntity;
class Quad;

/*
provide new class FieldBlock
- which supports Layout (see RandomGenerator)
- inheriates QuadEntity (or has a impl of it)
- provides setPos/Color functions
- setPos is overloaded, so the signal in QuadEntity can be erased
*/

class Field
{
private:
    class FieldCell
    {
        std::shared_ptr<QuadEntity> m_currentEntity;
        size_t m_currentQuad; //TODO: use indexes instead of pointer
    public:
        //FieldCell& operator=(const FieldCell& rhs){m_currentEntity = rhs.m_currentEntity; m_currentQuad = rhs.m_currentQuad;return *this;}
        FieldCell() :m_currentEntity(0), m_currentQuad(0){}
        FieldCell(std::shared_ptr<QuadEntity> e, Quad* q) :m_currentEntity(e), m_currentQuad(0){}

        bool isEmpty()const { return m_currentEntity == nullptr; }
        bool hasEntity(const QuadEntity* q)const { return (q == m_currentEntity.get()); }
        void reset() { m_currentEntity.reset(); m_currentQuad = 0; }
        /*  void swap(FieldCell& rhs)
          {
          std::swap(m_currentEntity,rhs.m_currentEntity);
          std::swap(m_currentQuad,rhs.m_currentQuad);
          }*/
        void setEntity(std::shared_ptr<QuadEntity> e)
        {
            m_currentEntity = e;
        }
        void setQuad(size_t q){ m_currentQuad = q; }
        size_t getQuad(){ return m_currentQuad; }
        std::shared_ptr<QuadEntity> getEntity(){ return m_currentEntity; }
    };



    //////////Methods//////////
public:
    /// add given entity to the field, not to the renderer
    Field(size_t rows, size_t columns, float x, float y, float quadSize);
    Field(const Field&) = delete;
    const Field& operator=(const Field&) = delete;
    ~Field();

    bool addQuadEntity(std::shared_ptr<QuadEntity> entity);

    //checks, if one action is valid, or if overlap exists, when action is executed
    bool isTranslationValid(const QuadEntity& entity, float x, float y, float z)const;
    bool isTranslationValid(const QuadEntity& entity, const float3&  translate)const;
    bool isZRotationValid(const QuadEntity& entity, const int direction)const;
    bool isZRotationValid(const QuadEntity& entity, const int direction, const float3& cog)const;

    /// get X and Y Position (lower left)
    float getX()const{ return m_width[0] - 0.5f*fieldSize().x; }
    float getY()const{ return m_height[0] - 0.5f*fieldSize().y; }
    //get dimension information coordinates given by the quad (not discrete values)
    float getWidth()const { return m_width[1] - m_width[0]; }
    float getHeight()const { return m_height[1] - m_height[0]; }
    float3 getCenter()const { return m_center; }
    float getLeftBound()const { return m_width[0]; }
    float getRightBound()const { return m_width[1]; }
    size_t getCellXCount()const { return m_cells.shape()[0]; }
    size_t getCellYCount()const { return m_cells.shape()[1]; }

    bool rowFull(const float y)const;
    bool rowEmpty(const float y)const;
    void swapRow(const float y1, const float y2);
    void swapRow(const size_t y1, const size_t y2);
    void eraseQuadsInRow(const float y);
    void rotateDown(float lastRow);
    void lockEntity(const QuadEntity& e); // TODO: remove method: quad cannot move, lock it

    struct FieldSize
    {
        float x;
        float y;
    };
    /// returns the size of one cell
    FieldSize fieldSize() const;

    /// erase all quad entities from the field
    void clear();


private:
    std::vector<std::shared_ptr<QuadEntity> > m_moveableEntities; // registers all movable entities
    std::map<std::tuple<const QuadEntity*,size_t>,FieldCell*> m_currentPositions; // hold the quad pointer of entities from m_moveableEntities and maps them to the current Cell

    boost::multi_array<FieldCell,2> m_cells;
    const float m_quadSize;
    float2 m_width,m_height;
    float3 m_center;

    //compute x/y position from quad coordinates to discrete coordinates
    size_t getY(const float y)const{return static_cast<size_t>(ceil(100*(y-m_height[0])/m_quadSize))/100;}
    size_t getX(const float x)const{return static_cast<size_t>(ceil(100*(x-m_width[0])/m_quadSize))/100;} 

    //compute x/y position to quad coordinates from discrete coordinates
    float toX(const size_t x)const{ return m_width[0]  + x*m_quadSize; }
    float toY(const size_t y)const{ return m_height[0] + y*m_quadSize; }

    /// get current cell on position in quad coordinates
    FieldCell& getCell(const float x,const float y){return m_cells[getY(y)][getX(x)];}
    const FieldCell& getCell(const float x,const float y)const{return m_cells[getY(y)][getX(x)];}
    void quadPositionChanged(const QuadEntity& entity); // track only

};
