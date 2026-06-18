#ifndef PREYDATOR_ENTITY_H__
#define PREYDATOR_ENTITY_H__

#include "../preydator_math.h"
#include "../preydator_config.h"
#include "../neural_network.h"
#include <cstdint>
#include <condition_variable>
#include <mutex>
#include <atomic>
#include <vector>

class Entity
{
public:
    Entity();
    Entity(NeuralNetworkPtr brain, unsigned int generation);
    Entity(Point location);
    Entity(NeuralNetworkPtr brain, unsigned int generation, Point location);
    virtual ~Entity() = default;
	void spawn();
	void spawn(int x, int y);
    void tick();
    void stop();
	virtual void action() = 0;
	virtual void perceive(InputLayerValues perception) = 0;
    int check();
	void sendSignal(int signal);
    virtual DrawInfo getDrawInfo() const = 0;
    int getId() const;
    int getFitness() const;
    int getLifetime() const;
	unsigned int getGeneration() const;
	NeuralNetwork& getBrain();
    Point getPreviousLocation() const;
    Point getLocation() const;
    Point getLocationRequest() const;
	void allowLocationUpdate();
	virtual std::string documentSelf() = 0;
	virtual std::string createSaveString() = 0;
	virtual double getPerceptionValue(Entity& entity) const = 0;
	virtual bool attack(Entity& entity) = 0;
	virtual std::unique_ptr<Entity> giveBirth(Point birthLocation) = 0;

protected:
	int calculateMovementBasedHealthReduction();

	int m_id;
	NeuralNetworkPtr m_brain;
	InputLayerValues m_perception;
	Point m_previous_location;
	Point m_location;
	Point m_location_request;
	Point m_desired_movement;
	bool m_allow_location_update;
	int m_health;
	int m_fitness;
	int m_lifetime;
	unsigned int m_reproduction;
	unsigned int m_generation;
	int m_signal;

};

using EntityPtr = std::unique_ptr<Entity>;

#endif /* PREYDATOR_ENTITY_H__ */