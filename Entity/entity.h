#ifndef PREYDATOR_ENTITY_H__
#define PREYDATOR_ENTITY_H__

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
    Entity(NeuralNetworkPtr brain);
    Entity(Point location);
    Entity(NeuralNetworkPtr brain, Point location);
    virtual ~Entity() = default;
	virtual void spawn() = 0;
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
	NeuralNetworkPtr getBrain() const;
    Point getLocation() const;
    Point getLocationRequest() const;
	void allowLocationUpdate();
	virtual std::string documentSelf() = 0;
	virtual std::string createSaveString() = 0;
	virtual double getPerceptionValue(std::shared_ptr<Entity> entity) const = 0;
	virtual bool attack(std::shared_ptr<Entity> entity) = 0;
	virtual std::shared_ptr<Entity> giveBirth(Point birthLocation) = 0;

protected:
    int m_id;
	NeuralNetworkPtr m_brain;
    Point m_location;
	Point m_location_request;
	Point m_desired_movement;
	bool m_allow_location_update;
    int m_health;
	int m_fitness;
	int m_lifetime;
    unsigned int m_reproduction;
    int m_signal;

};

using EntityPtr = std::shared_ptr<Entity>;

#endif /* PREYDATOR_ENTITY_H__ */