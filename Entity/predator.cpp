#include "predator.h"
#include "prey.h"

void Predator::spawn()
{
    m_location.x = std::rand() % (WORLD_X - 9);
    m_location.y = std::rand() % (WORLD_Y - 9);
	m_location_request.x = m_location.x + (std::rand() % 9) - 4;
	m_location_request.y = m_location.y + (std::rand() % 9) - 4;
}

void Predator::action()
{
    m_health -= std::rand() % 3;
	if(m_health <= 0) m_signal |= 0x1;
	if (m_allow_location_update)
	{
		m_location.x = m_location_request.x;
		m_location.y = m_location_request.y;
	}
	m_location_request.x = m_location.x + (std::rand() % 9) - 4;
	m_location_request.y = m_location.y + (std::rand() % 9) - 4;
	m_allow_location_update = false;
}

DrawInfo Predator::getDrawInfo() const
{
    return predatorDrawInfo;
}

bool Predator::attack(std::shared_ptr<Entity> entity)
{
	std::shared_ptr<Prey> prey = std::dynamic_pointer_cast<Prey>(entity);
	if (prey && !(prey->check() & 0x1))
	{
		prey->sendSignal(1);
		m_health += 400;
		m_reproduction += 2000;
		return true;
	}
    return false;
}

std::shared_ptr<Entity> Predator::giveBirth(Point birthLocation)
{
	m_reproduction = 0;
	return std::make_shared<Predator>(m_config, birthLocation);
}