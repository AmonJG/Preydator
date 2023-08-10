#include "entity.h"

Entity::Entity(DrawInfo const& drawInfo)
    : m_drawInfo(drawInfo)
{
    
}

Entity::Entity(DrawInfo const& drawInfo, Point location)
    : m_drawInfo(drawInfo), m_location(location)
{
    
}

Entity::~Entity()
{
    
}

DrawInfo Entity::getDrawInfo()
{
    return m_drawInfo;
}

