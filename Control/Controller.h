
#pragma once
#include <vector>
#include <utility>

#include <utility/Vector.hpp>


class SimpleTetris;
class MainWindow;

class Controller
{
    SimpleTetris& m_game;
    size_t m_id;
	std::vector<int2> m_cursors;
public:

    ////////////////////////////////////////////
    /// gamestate
    bool isPaused()const;
    void pause();
    void reset();
    void start();


    ////////////////////////////////////////////
    /// control
    void rotateZ(int direction);
    void translate(const float3& translation);
    void translateDown();
    void translateRight();
    void translateLeft();
    void dump();

	void showCursors(std::vector<int2> c) { m_cursors = c; }

    ////////////////////////////////////////////
    /// game information
    /// returns the current position in 3D space
    float3 currentCog() const ;

    /// returns selected QuadID or 0, if nothing was picked
    size_t getQuadID(float2 p);

    /** \brief initialize a controller
    @param game             current game, which will be controlled
    @param controllerId id  of the current controller
    */
    Controller(SimpleTetris& game, size_t controllerId) :m_game(game), m_id(controllerId){}
};