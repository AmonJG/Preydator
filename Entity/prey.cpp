#include "prey.h"

void Prey::action()
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
	if(std::rand() % 50 == 0) m_reproduction += 400;
}

void Prey::perceive(InputLayerValues perception)
{
	m_perception = perception;
}

DrawInfo Prey::getDrawInfo() const
{
    return preyDrawInfo;
}

double Prey::getPerceptionValue(Entity& entity) const
{
	return dynamic_cast<Prey*>(&entity) ?
	PREY_SEEN_BY_PREY_INPUT_LAYER_VALUE :
	PREY_SEEN_BY_PREDATOR_INPUT_LAYER_VALUE;
}

bool Prey::attack(Entity& entity)
{
    return false;
}

EntityPtr Prey::giveBirth(Point birthLocation)
{
	m_reproduction = 0;
	m_fitness++;
	return std::make_unique<Prey>(std::make_unique<NeuralNetwork>(*m_brain), m_generation + 1, birthLocation);
}

std::string Prey::documentSelf()
{
	return m_brain->exportGraph("Prey");
}

std::string Prey::createSaveString()
{
	return m_brain->getSaveString("Prey");
}