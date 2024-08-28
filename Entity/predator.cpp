#include "predator.h"

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
	if (m_allow_location_request)
	{
		m_location.x = m_location_request.x;
		m_location.y = m_location_request.y;
	}
	m_location_request.x = m_location.x + (std::rand() % 9) - 4;
	m_location_request.y = m_location.y + (std::rand() % 9) - 4;
	m_allow_location_request = false;
}

DrawInfo Predator::getDrawInfo() const
{
    return predatorDrawInfo;
}