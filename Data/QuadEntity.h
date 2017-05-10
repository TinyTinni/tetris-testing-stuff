#pragma once

#include <array>
#include <iterator>
#include <cassert>
#include <functional>

#include <utility/Vector.hpp>

#include <boost/signals2.hpp>

/**
   guarantees continuos data of all quads in the following layout:
   position [0-2]
   color[4-6]
*/

class Quad
{
private:
    typedef std::array<float, 8> array;
    array m_data;
public:
    Quad(){}
    float3 position()const{float3 result;result[0]=m_data[0];result[1]=m_data[1];result[2]=m_data[2];return result;}
    float3 color()const{float3 result;result[0]=m_data[4];result[1]=m_data[5];result[2]=m_data[6];return result;}
    
    void setPosition(const float3& rhs){std::copy(rhs.begin(),rhs.end(),m_data.begin());}
    void setColor(const float3& rhs){std::copy(rhs.begin(),rhs.end(),m_data.begin()+4);}

    void setPosition(float x, float y, float z){m_data[0] = x; m_data[1]= y; m_data[2] = z;}
    void setColor(float r, float g, float b){m_data[4] = r; m_data[5] = g; m_data[6] = b;}
    
    const array& data()const{ return m_data; }
    float& operator[](size_t n){return m_data[n];}
    const float& operator[](size_t n)const{return m_data[n];}

    static const size_t size(){ return std::tuple_size<array>::value; }
    static const float sideLength(){return 0.1f;}//remember to ajust the sideLength also in the geometry shader
};



/**

    QuadGroup, holds multiple quds in one layout
    is intended to be the communicator between the gameplay layout
    and the renderer.
*/
class FieldGLBuffer;

class QuadEntity
{
private:
    size_t m_size;
    /** DON'T use this member to get access of Quads. Use m_quadsProxy instead, beacuse m_quadsRawPtr is sometimes not a valid ptr
    * m_quadsRawPtr stores the quadarray. if no renderer is registered, it is a valid ptr to an array. if renderer is registered, it stores the offset
    * for the data inside the renderer internal buffer. Use m_quadsProx() to get save access on the Quads
    */

    Quad* m_quadsRawPtr; //todo: use union Quad* and ptrdiff_t
    FieldGLBuffer* m_buffer;

    mutable boost::signals2::signal<void (const QuadEntity&)> m_positionChangedSignal;

    Quad* m_quadsProxy()const;

public:

    /// iterators
    class ConstPositionIterator : public std::iterator<std::bidirectional_iterator_tag,float3 >
    {
        const QuadEntity* m_entity;
        const Quad* m_currentQuad;
    public:
        ConstPositionIterator(const QuadEntity *qe):m_entity(qe),m_currentQuad(qe->begin()){}
        ConstPositionIterator(const QuadEntity *qe,const Quad* current):m_entity(qe),m_currentQuad(current){}        

        ConstPositionIterator operator++(int)
        {
            return ConstPositionIterator(m_entity,m_currentQuad++);
        }
        ConstPositionIterator operator--(int)
        {
            return ConstPositionIterator(m_entity,m_currentQuad--);
        }
        ConstPositionIterator operator--()
        {
            --m_currentQuad;
            return *this;
        }
        ConstPositionIterator operator++()
        {
            ++m_currentQuad;
            return *this;
        }
        value_type operator*()
        {
            return m_currentQuad->position();
        }
        value_type operator->() const
        {
            return m_currentQuad->position();
        }
        bool operator==(const ConstPositionIterator &rhs)
        {
            bool result = ((m_entity == rhs.m_entity) && (m_currentQuad == rhs.m_currentQuad));
            return result;
        }
        bool operator!=(const ConstPositionIterator &rhs)
        {
            return !(*this==rhs);
        }
        //const reference operator=(const reference &rhs)
        //{
        //    m_currentQuad->setPosition(rhs);
        //    return rhs;
        //}
    };

    class ColorIterator //todo
    {
    public:
    };
/////////////////////////////////////////////////////
    /// functions
    QuadEntity(size_t maxSize):m_size(maxSize),m_quadsRawPtr(new Quad[m_size]),m_buffer(0){}
    ~QuadEntity(){clear();}
    size_t size() const {return m_size;}

    /// clones the current Quad positions and returns the cloned Quad. Cloned Quad does not own any Renderer
    std::shared_ptr<QuadEntity> clonePosFree()const
    {
        std::shared_ptr<QuadEntity> result = std::make_shared<QuadEntity>(m_size);
        auto iter2 = result->begin();
        for (auto iter = beginPosition(); iter != endPosition(); ++iter, ++iter2)
            iter2->setPosition(*iter);
        return result;
    }

    /// remove all quads and unregister to current renderer
    void clear();

    void erase(const Quad* quad);
    void erase(const size_t first,const size_t numElements);

    /// call "func" if entity's position was changed. parameter of func is the current entity
    void informPositionChanged(std::function<void (const QuadEntity&)> func)const{m_positionChangedSignal.connect(func);}

    Quad& operator[](size_t n){assert(m_size>n);return m_quadsProxy()[n];}
    const Quad& operator[](size_t n)const{assert(m_size>n);return m_quadsProxy()[n];}

    Quad* begin(){return m_quadsProxy();}
    const Quad* begin()const{return m_quadsProxy();}
    Quad* end(){return m_quadsProxy()+m_size;}
    const Quad* end()const{return m_quadsProxy()+m_size;}

    ConstPositionIterator beginPosition()const{return ConstPositionIterator(this,begin());}
    ConstPositionIterator endPosition()const{return ConstPositionIterator(this,end());}


    /// translate all quads
    void translate(const float x,const float y,const float z);
    void translate(const float3& tr){translate(tr[0],tr[1],tr[2]);}
    /// rotate Z axis with origin = cog()
    void rotateZ(const int direction); 
    /// rotate Z axis with specified origin
    void rotateZ(const int direction, const float3& origin); 
    /// return center of gravity
    float3 cog() const;

    /// return lowest y value of all quads
    float lowerBound()const ;
    /** return lowest y value of all quads with centric coordinate x. 
    *   returns NaN, if no Quad has the given x coordinate
    *   @param x coordidnate of the checked quads
    *   @param checkdigits give the number of digits after . which will be checked of x
    */
    float lowerBound(float x, unsigned checkDigits = 5) const;
    /// return highest y value of all quads
    float upperBound()const ;
    /// return highest x value
    float rightBound()const ;
    /// return lowest x value
    float leftBound()const ;

    /// binds the entity to a (e.g. graphics) buffer
    void registerBuffer(FieldGLBuffer* buffer, Quad *newBufferPos);
    /// checks, if the passed buffer is equal with the registered
    bool checkBuffer(const FieldGLBuffer* renderer)const;
    /// get raw ptr
    Quad* getRawPtr()const {return m_quadsRawPtr;} 
    

};
