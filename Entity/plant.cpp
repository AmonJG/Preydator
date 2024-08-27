#include "plant.h"

void Plant::spawn()
{
    m_location.x = std::rand() % (WORLD_X - 9);
    m_location.y = std::rand() % (WORLD_Y - 9);
}

void Plant::action()
{
    m_health -= std::rand() % 3;
}

DrawInfo Plant::getDrawInfo() const
{
    return plantDrawInfo;
}