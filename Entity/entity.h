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
    Entity(NeuralNetwork brain);
    Entity(Point location);
    Entity(NeuralNetwork brain, Point location);
    virtual ~Entity() = default;
	virtual void spawn() = 0;
    void run(std::atomic<int>& stopFlag);
    void stop();
	virtual void action() = 0;
	virtual void perceive(InputLayerValues perception) = 0;
    int check();
    void setSharedData(std::condition_variable* cv, std::mutex* mtx, bool* h1, bool* h2);
    void sendSignal(int signal);
    virtual DrawInfo getDrawInfo() const = 0;
    int getId() const;
	NeuralNetwork getBrain() const;
    Point getLocation() const;
    Point getLocationRequest() const;
	void allowLocationUpdate();
	virtual void documentSelf() = 0;
	virtual std::string createSaveString() = 0;
	virtual double getPerceptionValue() const = 0;
	virtual bool attack(std::shared_ptr<Entity> entity) = 0;
	virtual std::shared_ptr<Entity> giveBirth(Point birthLocation) = 0;

protected:
    int m_id;
	NeuralNetwork m_brain;
    Point m_location;
	Point m_location_request;
	Point m_desired_movement;
	bool m_allow_location_update;
    int m_health;
    unsigned int m_reproduction;
    std::condition_variable* mp_cv;
    std::mutex* mp_mtx;
    bool* mp_haltAgents;
    bool* mp_haltAgents2;
    int m_signal;

};

#endif /* PREYDATOR_ENTITY_H__ */