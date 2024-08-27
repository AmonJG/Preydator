#include "predator.h"

void Predator::spawn()
{
    m_location.x = std::rand() % (WORLD_X - 9);
    m_location.y = std::rand() % (WORLD_Y - 9);
}

void Predator::action()
{
	m_location.x += (std::rand() % 9) - 4;
    m_location.y += (std::rand() % 9) - 4;
    m_health -= std::rand() % 2;
}

DrawInfo Predator::getDrawInfo() const
{
    return predatorDrawInfo;
}