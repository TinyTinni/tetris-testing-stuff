#include "Field.h"
#include "QuadEntity.h"

#include <algorithm>


Field::FieldSize Field::fieldSize()const
{
    FieldSize result = {m_quadSize,m_quadSize};
    return result;
}

Field::Field(size_t rows, size_t columns, float x, float y, float quadSize)
    : m_moveableEntities(),
    m_currentPositions(),
    m_cells(boost::extents[rows][columns]),
    m_quadSize(quadSize)
{
    m_center[0] = x;
    m_center[1] = y;
    m_center[2] = -2.f;
    m_width[0] = x - columns*0.5f*quadSize;
    m_width[1] = x + columns*0.5f*quadSize;
    m_height[0] = x - rows*0.5f*quadSize;
    m_height[1] = y + rows*0.5f*quadSize;

    for (size_t i = 0; i < rows; ++i)
        for (size_t j = 0; j < columns; ++j)
            m_cells[i][j] = FieldCell();
}

bool Field::addQuadEntity(std::shared_ptr<QuadEntity> entity)
{
    // check, if space is free
    for (Quad* iter = entity->begin(); iter != entity->end() ;++iter)
    {
        float3 pos = iter->position();
        FieldCell& cell = getCell(pos[0],pos[1]);
        if (!cell.isEmpty())
            return false;
    }

    // space is free, so add the quad to the cells and set callback functions
    for (size_t i = 0; i < entity->size(); ++i)
    {
        float3 pos = (*entity)[i].position();
        FieldCell& cell = getCell(pos[0],pos[1]);
        cell.setQuad(i);
        cell.setEntity(entity);
        assert(!cell.isEmpty());
        m_currentPositions[std::make_tuple(entity.get(),i)] = &cell;
    }

    m_moveableEntities.push_back(entity);
    entity->informPositionChanged([this](const QuadEntity& e){this->quadPositionChanged(e);});
    return true;
}

void Field::quadPositionChanged(const QuadEntity& entity)
{
    //search for the entity. if its not in m_moveableEntities, the quads are not in m_currentPositions
    auto iter = m_moveableEntities.begin();//std::find(m_moveableEntities.begin(),m_moveableEntities.end(), &entity);
    if (iter == m_moveableEntities.end())
        return;

    //m_cells.begin()->

    //m_cells.std::remove_if(std::begin(m_cells),std::end(m_cells,)[&entity](const auto& it){return it->hasEntity(&entity);});
    for (auto it = m_cells.origin(); it != m_cells.origin() + m_cells.num_elements(); ++it)
    {
        //if (it->hasEntity(&entity))
         //   it->reset();
    }


    // sort quads, min y first
    /*std::vector<std::tuple<const Quad*,size_t> > quadVec;
    for (size_t i = 0;i < entity.size(); ++i)
        quadVec.emplace_back(&entity[i],i);

    std::sort(quadVec.begin(),quadVec.end(),
        []  (std::tuple<const Quad*,size_t> rhs, std::tuple<const Quad*,size_t> lhs)
        {
            return (std::get<0>(rhs)->position()[1] < std::get<0>(lhs)->position()[1]);
        });
    */
    // translate
    std::shared_ptr<QuadEntity> entityPtr;
    //remove old data
    for (size_t i = 0; i< entity.size(); ++i)
    {
        auto key = std::make_tuple(&entity,i);
        entityPtr = m_currentPositions[key]->getEntity();
        m_currentPositions[key]->reset();
    }
    //set new translated data
    for (size_t i = 0;i < entity.size(); ++i)
    {
        const Quad* quad = &entity[i];
        FieldCell& currentCell = getCell(quad->position()[0],quad->position()[1]);
        currentCell.setEntity(entityPtr);
        //assert(!entityPtr->isEmpty());
        currentCell.setQuad(i);
        //currentCell = oldCell;
        //oldCell.reset();
        auto key = std::make_tuple(&entity,i);
        m_currentPositions[key] = &currentCell;    
    }
}

bool Field::rowFull(const float y)const
{
    const size_t rowIndex = getY(y);
    assert (rowIndex < m_cells.size());
    return !std::any_of(m_cells[rowIndex].begin(),m_cells[rowIndex].end(),std::mem_fn(&FieldCell::isEmpty));
}

bool Field::rowEmpty(const float y)const
{
    const size_t rowIndex = getY(y);
    assert (rowIndex < m_cells.size());
    return std::all_of(m_cells[rowIndex].begin(),m_cells[rowIndex].end(),std::mem_fn(&FieldCell::isEmpty));
}

void Field::eraseQuadsInRow(const float y)
{
    const size_t rowIndex = getY(y);
    assert (rowIndex < m_cells.size());
    for (auto iter = m_cells[rowIndex].begin(); iter != m_cells[rowIndex].end(); ++iter)
    {
        if ((*iter).isEmpty()) // could not use ->... maybe a strange boost bug?
            continue;
        std::shared_ptr<QuadEntity> e = (*iter).getEntity();
        size_t quadnr = (*iter).getQuad();
        (*e)[(*iter).getQuad()].setColor(0.f,0.f,0.f);
        //e->erase((*iter).getQuad(),1);
        (*iter).reset();
    }
}

bool Field::isTranslationValid(const QuadEntity& entity, float x, float y, float z)const
{
    float3 translate;
    translate[0] = x;
    translate[1] = y;
    translate[2] = z;
    return isTranslationValid(entity,translate);
}

bool Field::isTranslationValid(const QuadEntity& entity, const float3&  translate)const
{
    for (const Quad* iter = entity.begin(); iter != entity.end() ; ++iter)
    {
        float3 pos = iter->position()+translate;

        size_t x = getX(pos[0]);
        size_t y = getY(pos[1]);

        // check if out of boundary
        if ((x >= m_cells[0].size()) || ( y >= m_cells.size()))
            return false;

        // check, if cells are free
        const FieldCell& cell = m_cells[y][x];
        if (!cell.isEmpty() && !cell.hasEntity(&entity))
            return false;
    }
    return true;
}


bool Field::isZRotationValid(const QuadEntity& entity,const int direction)const
{
    return isZRotationValid(entity,direction,entity.cog());
}

bool Field::isZRotationValid(const QuadEntity& entity,const int direction, const float3& cog)const
{
    std::shared_ptr<QuadEntity> newQuad = entity.clonePosFree();
    newQuad->rotateZ(direction,cog);
    for (const Quad* iter = newQuad->begin(); iter != newQuad->end() ; ++iter)
    {
        float3 pos = iter->position();

        size_t x = getX(pos[0]);
        size_t y = getY(pos[1]);

        // check if out of boundary
        if ((x >= m_cells[0].size()) || ( y >= m_cells.size()))
            return false;

        // check, if cells are free
        const FieldCell& cell = m_cells[y][x];
        if (!cell.isEmpty() && !cell.hasEntity(&entity))
            return false;
    }
    return true;
}

void Field::lockEntity(const QuadEntity& e)
{
    for (auto pos = e.beginPosition(); pos != e.endPosition(); ++pos)
        const FieldCell& cell = getCell((*pos)[0],(*pos)[1]);
    m_moveableEntities.clear();
    //m_moveableEntities.erase(std::find(m_moveableEntities.begin(),m_moveableEntities.end(),&e));
    //for (size_t i = 0; i < e.size(); ++i)
        //m_currentPositions.erase(std::make_tuple(&e,i));
}

void Field::swapRow(const float y1, const float y2)
{
    size_t row1 = getY(y1), row2 = getY(y2);
}
void Field::swapRow(const size_t row1, const size_t row2)
{
    float y1 = toY(row1);
    float y2 = toY(row2);
    assert(row1 < m_cells.size() && row2 < m_cells.size());
    for (size_t x = 0; x < m_cells[row1].size(); ++x)
    {
        if (m_cells[row1][x].getEntity())
        {
            auto& q = (*m_cells[row1][x].getEntity())[m_cells[row1][x].getQuad()];
            float3 pos1 = q.position();
            pos1[1] = y2;
            q.setPosition(pos1);
        }

        if (m_cells[row2][x].getEntity())
        {
            auto& q = (*m_cells[row2][x].getEntity())[m_cells[row2][x].getQuad()];
            float3 pos2 = q.position();
            pos2[1] = y1;
            q.setPosition(pos2);
        }
        std::swap(m_cells[row1][x], m_cells[row2][x]);
    }
    //std::swap(m_cells[row1], m_cells[row2]);

}

void Field::rotateDown(float lastRow)
{
    //auto endIter = m_cells.begin() + getY(lastRow);
    size_t i2 = getY(lastRow);
    for (size_t i = getY(lastRow); i < m_cells.size() - 1; ++i)
        swapRow(i, i + 1);
}

void Field::clear()
{
    m_moveableEntities.clear();
    m_currentPositions.clear();
    for (size_t x = 0; x < m_cells.size(); ++x)
    for (size_t y = 0; y < m_cells[x].size(); ++y)
        m_cells[x][y].reset();
}

Field::~Field()
{
}
