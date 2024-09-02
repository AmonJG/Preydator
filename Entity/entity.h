#ifndef PREYDATOR_ENTITY_H__
#define PREYDATOR_ENTITY_H__

#include "../preydator_config.h"
#include <cstdint>
#include <condition_variable>
#include <mutex>
#include <atomic>
#include <vector>

#define WORLD_X 1900
#define WORLD_Y 1000

struct Point
{
    int x, y;
};

struct Color
{
	uint8_t r, g, b, a;
};

struct DrawInfo
{
    std::vector<Point> points;
    Color color;
};

class Entity
{
public:
    Entity(PreydatorConfig config);
    Entity(PreydatorConfig config, Point location);
    virtual ~Entity() = default;
	virtual void spawn() = 0;
    void run(std::atomic<int>& stopFlag);
    void stop();
	virtual void action() = 0;
    int check();
    void setSharedData(std::condition_variable* cv, std::mutex* mtx, bool* h1, bool* h2);
    void sendSignal(int signal);
    virtual DrawInfo getDrawInfo() const = 0;
    int getId() const;
    Point getLocation() const;
    Point getLocationRequest() const;
	void allowLocationUpdate();
	virtual bool attack(std::shared_ptr<Entity> entity) = 0;
	virtual std::shared_ptr<Entity> giveBirth(Point birthLocation) = 0;

protected:
    int m_id;
	const PreydatorConfig m_config;
    Point m_location;
	Point m_location_request;
	bool m_allow_location_update;
    int m_health;
    int m_reproduction;
    std::condition_variable* mp_cv;
    std::mutex* mp_mtx;
    bool* mp_haltAgents;
    bool* mp_haltAgents2;
    int m_signal;

};

#endif /* PREYDATOR_ENTITY_H__ */