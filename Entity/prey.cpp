#include "prey.h"
#include "plant.h"

void Prey::spawn()
{
    m_location.x = std::rand() % (WORLD_X - 9);
    m_location.y = std::rand() % (WORLD_Y - 9);
	m_location_request.x = m_location.x + (std::rand() % 5) - 2;
	m_location_request.y = m_location.y + (std::rand() % 5) - 2;
}

void Prey::action()
{
    m_health -= std::rand() % 3;
	if (m_allow_location_update)
	{
		m_location.x = m_location_request.x;
		m_location.y = m_location_request.y;
	}
	m_location_request.x = m_location.x + (std::rand() % 5) - 2;
	m_location_request.y = m_location.y + (std::rand() % 5) - 2;
	m_allow_location_update = false;
}

DrawInfo Prey::getDrawInfo() const
{
    return preyDrawInfo;
}

bool Prey::attack(std::shared_ptr<Entity> entity)
{
	std::shared_ptr<Plant> plant = std::dynamic_pointer_cast<Plant>(entity);
	if (plant)
	{
		plant->sendSignal(1);
		m_health += 200;
		return true;
	}
    return false;
}