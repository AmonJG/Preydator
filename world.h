#ifndef PREYDATOR_WORLD_H__
#define PREYDATOR_WORLD_H__

#include "Entity/entity.h"
#include "Entity/barrier.h"
#include "graphics_handler.h"
#include <condition_variable>
#include <mutex>
#include <atomic>
#include <thread>
#include <vector>

using EntityPtr = std::shared_ptr<Entity>;

typedef struct {
    EntityPtr entity;
    std::thread thread;
} Agent;

class World
{
public:
    World(std::vector<EntityPtr> entities, std::vector<Barrier> barriers);
    ~World();
    std::vector<EntityPtr> getEntities();
    void startAgents();
    void stopAgents();
    bool alive();
    void tick();
    void updateAgents();
    void drawEntities();

private:
	void initializeBlockedSpaceArray();
	bool blockedSpace[WORLD_X][WORLD_Y] = {0};
    GraphicsHandler* m_graphicsHandler;
    std::vector<EntityPtr> m_entities;
	std::vector<Barrier> m_barriers;
    std::condition_variable m_cv;
    std::mutex m_mtx;
    std::vector<Agent> m_agents;
    std::atomic<int> m_stopFlag;
    bool m_haltAgents = true;
    bool m_haltAgents2 = true;
};

#endif /* PREYDATOR_WORLD_H__ */