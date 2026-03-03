#include "entity.h"
#include <cstdlib>
#include <iostream>

static int newId()
{
    static int id = 0;
    return ++id;
}

Entity::Entity()
{
    m_id = newId();
	m_allow_location_update = true;
    m_health = config.entity_start_health;
	m_fitness = 0;
	m_lifetime = 0;
    m_reproduction = 0;
    m_signal = 0;
}

Entity::Entity(NeuralNetworkPtr brain, unsigned int generation)
	: m_brain(std::move(brain)), m_generation(generation)
{
    m_id = newId();
	m_allow_location_update = true;
    m_health = config.entity_start_health;
	m_fitness = 0;
	m_lifetime = 0;
    m_reproduction = 0;
    m_signal = 0;
}

Entity::Entity(Point location)
{
    m_id = newId();
	m_allow_location_update = true;
    m_health = config.entity_start_health;
	m_fitness = 0;
	m_lifetime = 0;
    m_reproduction = 0;
    m_signal = 0;
	m_location.x = location.x;
	m_location.y = location.y;
	m_location_request.x = m_location.x;
	m_location_request.y = m_location.y;
}

Entity::Entity(NeuralNetworkPtr brain, unsigned int generation, Point location)
	: m_brain(std::move(brain)), m_generation(generation)
{
    m_id = newId();
	m_allow_location_update = true;
    m_health = config.entity_start_health;
	m_fitness = 0;
	m_lifetime = 0;
    m_reproduction = 0;
    m_signal = 0;
	m_location.x = location.x;
	m_location.y = location.y;
	m_location_request.x = m_location.x;
	m_location_request.y = m_location.y;
}

void Entity::tick()
{
    if (m_signal != 0) return;
    action();
}

int Entity::check()
{
    int flags = 0x0;
    if(m_health <= 0) flags |= 0x1;
    if(m_reproduction >= config.entity_reproduction_goal) flags |= 0x2;
    return flags |= m_signal;
}

void Entity::sendSignal(int signal)
{
    m_signal = signal;
}

int Entity::getId() const
{
    return m_id;
}

int Entity::getFitness() const
{
    return m_fitness;
}

int Entity::getLifetime() const
{
    return m_lifetime;
}

unsigned int Entity::getGeneration() const
{
    return m_generation;
}

NeuralNetwork& Entity::getBrain()
{
    return *m_brain;
}

Point Entity::getPreviousLocation() const
{
    return m_previous_location;
}

Point Entity::getLocation() const
{
    return m_location;
}

Point Entity::getLocationRequest() const
{
    return m_location_request;
}

void Entity::allowLocationUpdate()
{
    m_allow_location_update = true;
}

int Entity::calculateMovementBasedHealthReduction()
{
	if (m_lifetime <= 1) return config.entity_standard_health_loss_per_tick;
	int std_loss = config.entity_standard_health_loss_per_tick;
	int health_reduction = gaussianNoise(std_loss, std_loss / 3);
	health_reduction = std::clamp(health_reduction, 0, std_loss * 2);
	int distance = getEucldeanDistance(
		m_location.x, m_location.y, m_previous_location.x, m_previous_location.y);
	health_reduction += distance * distance;
	return health_reduction;
}