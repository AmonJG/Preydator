#include "predator.h"
#include "prey.h"


#include <iostream>

void Predator::action()
{
	m_desired_movement = m_brain->decideMovement(m_perception, {m_previous_location, m_location, m_health});
    m_health -= calculateMovementBasedHealthReduction();
	if(m_health <= 0) m_signal |= 0x1;
	// Only if move is allowed (because it got updated by world in the
	// beginning of the tick cycle), location gets updated by the last
	// valid location request and previouse location gets saved.
	if (m_allow_location_update)
	{
		m_previous_location.x = m_location.x;
		m_previous_location.y = m_location.y;
		m_location.x = m_location_request.x;
		m_location.y = m_location_request.y;
	}
	else // Lose health for invalid move
	{
		m_health -= config.entity_standard_health_loss_per_tick * 2;
	}
	// New desired movement is already known at this point and is checked
	// for validity in the beginning of the new tick cycle.
	m_location_request.x = mod((m_location.x + m_desired_movement.x), WORLD_X);
	m_location_request.y = mod((m_location.y + m_desired_movement.y), WORLD_Y);
	m_allow_location_update = false;
	m_lifetime++;
}

void Predator::perceive(InputLayerValues perception)
{
	m_perception = perception;
}

DrawInfo Predator::getDrawInfo() const
{
    return predatorDrawInfo;
}

double Predator::getPerceptionValue(Entity& entity) const
{
	return dynamic_cast<Prey*>(&entity) ?
	PREDATOR_SEEN_BY_PREY_INPUT_LAYER_VALUE :
	PREDATOR_SEEN_BY_PREDATOR_INPUT_LAYER_VALUE;
}

bool Predator::attack(Entity& entity)
{
	Prey* prey = dynamic_cast<Prey*>(&entity);
	if (prey && !(prey->check() & 0x1))
	{
		prey->sendSignal(1);
		m_health += 2000;
		m_reproduction += 2000;
		return true;
	}
    return false;
}

EntityPtr Predator::giveBirth(Point birthLocation)
{
	m_reproduction = 0;
	m_fitness++;
	return std::make_unique<Predator>(std::make_unique<NeuralNetwork>(*m_brain), m_generation + 1, birthLocation);
}

std::string Predator::documentSelf()
{
	return m_brain->exportGraph("Predator");
}

std::string Predator::createSaveString()
{
	return m_brain->getSaveString("Predator");
}