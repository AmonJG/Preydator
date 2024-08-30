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
#include <fstream>

using EntityPtr = std::shared_ptr<Entity>;

typedef struct {
    EntityPtr entity;
    std::thread thread;
} Agent;

class World
{
public:
	World(World &other) = delete;
    void operator=(World const&) = delete;
    static World* GetInstance(std::vector<EntityPtr> entities, std::vector<EntityPtr> barriers);
    std::vector<EntityPtr> getEntities();
	void initializeBarriers();
    void startAgents();
    void stopAgents();
    bool alive();
    void tick();
    void updateAgents();
    void drawEntities();

protected:
    World(std::vector<EntityPtr> entities, std::vector<EntityPtr> barriers);
    ~World();

private:
	void startEntityAgent(EntityPtr entity);
	bool validMove(EntityPtr entity) const;
	void updateEntityLocation(EntityPtr entity);
	Point getBirthLocation(EntityPtr parent);
    static World* m_worldSingletonInstance;
    static std::mutex m_constructorMutex;
    GraphicsHandler* m_graphicsHandler;
    std::vector<EntityPtr> m_entities;
	std::vector<EntityPtr> m_barriers;
    std::condition_variable m_cv;
    std::mutex m_mtx;
    std::vector<Agent> m_agents;
    std::atomic<int> m_stopFlag;
    bool m_haltAgents = true;
    bool m_haltAgents2 = true;
};

#endif /* PREYDATOR_WORLD_H__ */