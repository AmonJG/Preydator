#include "plant.h"




#include <iostream>

void Plant::spawn()
{
    m_location.x = std::rand() % (WORLD_X - 9);
    m_location.y = std::rand() % (WORLD_Y - 9);
}

void Plant::action()
{
    m_health -= std::rand() % 3;
	if(m_health <= 0) m_signal |= 0x1;
    m_reproduction += std::rand() % 10;
}

DrawInfo Plant::getDrawInfo() const
{
    return plantDrawInfo;
}

bool Plant::attack(std::shared_ptr<Entity> entity)
{
    return false;
}

std::shared_ptr<Entity> Plant::giveBirth(Point birthLocation)
{
	m_reproduction = 0;
	return std::make_shared<Plant>(m_config, birthLocation);
}