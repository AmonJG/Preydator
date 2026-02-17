#include "prey.h"
#include "plant.h"

void Prey::spawn()
{
    m_location.x = std::rand() % (WORLD_X - 9);
    m_location.y = std::rand() % (WORLD_Y - 9);
	m_location_request.x = m_location.x;
	m_location_request.y = m_location.y;
}

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
		m_health -= config.entity_standard_health_loss_per_tick * 5;
	}
	// New desired movement is already known at this point and is checked
	// for validity in the beginning of the new tick cycle.
	m_location_request.x = m_location.x + m_desired_movement.x;
	m_location_request.y = m_location.y + m_desired_movement.y;
	m_allow_location_update = false;
	m_lifetime++;
}

void Prey::perceive(InputLayerValues perception)
{
	m_perception = perception;
}

DrawInfo Prey::getDrawInfo() const
{
    return preyDrawInfo;
}

double Prey::getPerceptionValue(EntityPtr entity) const
{
	return std::dynamic_pointer_cast<Prey>(entity) ?
	PREY_SEEN_BY_PREY_INPUT_LAYER_VALUE :
	PREY_SEEN_BY_PREDATOR_INPUT_LAYER_VALUE;
}

bool Prey::attack(EntityPtr entity)
{
	std::shared_ptr<Plant> plant = std::dynamic_pointer_cast<Plant>(entity);
	if (plant && !(plant->check() & 0x1))
	{
		plant->sendSignal(1);
		m_health += 400;
		m_reproduction += 2000;
		return true;
	}
    return false;
}

EntityPtr Prey::giveBirth(Point birthLocation)
{
	m_reproduction = 0;
	m_fitness++;
	return std::make_shared<Prey>(std::make_shared<NeuralNetwork>(*m_brain), birthLocation);
}

std::string Prey::documentSelf()
{
	return m_brain->exportGraph("Prey");
}

std::string Prey::createSaveString()
{
	return m_brain->getSaveString("Prey");
}