#include "plant.h"
#include "prey.h"



#include <iostream>

void Plant::spawn()
{
    m_location.x = std::rand() % (WORLD_X - 9);
    m_location.y = std::rand() % (WORLD_Y - 9);
}

void Plant::action()
{
	if(std::rand() % 50 == 0) m_health -= 400;
	if(m_health <= 0) m_signal |= 0x1;
    if(std::rand() % 50 == 0) m_reproduction += 400;
	m_lifetime++;
}

void Plant::perceive(InputLayerValues perception)
{
}

DrawInfo Plant::getDrawInfo() const
{
    return plantDrawInfo;
}

double Plant::getPerceptionValue(Entity& entity) const
{
	return dynamic_cast<Prey*>(&entity) ?
		PLANT_SEEN_BY_PREY_INPUT_LAYER_VALUE :
		PLANT_SEEN_BY_PREDATOR_INPUT_LAYER_VALUE;
}

bool Plant::attack(Entity& entity)
{
    return false;
}

EntityPtr Plant::giveBirth(Point birthLocation)
{
	m_reproduction = 0;
	return std::make_unique<Plant>(birthLocation);
}

std::string Plant::documentSelf()
{
	return "";
}

std::string Plant::createSaveString()
{
	return "";
}