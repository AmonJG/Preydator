#include "prey.h"

void Prey::spawn()
{
    m_location.x = std::rand() % (WORLD_X - 9);
    m_location.y = std::rand() % (WORLD_Y - 9);
}

void Prey::action()
{
	m_location.x += (std::rand() % 5) - 2;
    m_location.y += (std::rand() % 5) - 2;
    m_health -= std::rand() % 3;
}

DrawInfo Prey::getDrawInfo() const
{
    return preyDrawInfo;
}