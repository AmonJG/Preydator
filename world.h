#ifndef PREYDATOR_WORLD_H__
#define PREYDATOR_WORLD_H__

#include "preydator_config.h"
#include "Entity/entity.h"
#include "Entity/barrier.h"
#include "graphics_handler.h"
#include <condition_variable>
#include <mutex>
#include <atomic>
#include <thread>
#include <vector>
#include <queue>

struct Agent
{
    EntityPtr entity;
    std::thread thread;
};

using AgentPtr = std::shared_ptr<Agent>;

struct EntityPtrCompare
{
    bool operator()(const EntityPtr& a, const EntityPtr& b) const
	{
        if (a->getFitness() != b->getFitness())
            return a->getFitness() < b->getFitness();
        return a->getLifetime() < b->getLifetime();
    }
};

class World
{
public:
	World(World &other) = delete;
    void operator=(World const&) = delete;
    static World* GetInstance();
    std::vector<EntityPtr> getEntities();
	void initNewGeneration();
	bool initSavedGeneration(std::string const& in_file_path);
	void killGeneration();
	bool generationAlive();
    void updateAgents();
    void drawEntities();

protected:
    World();
    ~World();

private:
	void initGeneration(std::vector<NeuralNetworkPtr> const& preyBrains, std::vector<NeuralNetworkPtr> const& predatorBrains);
	void initializeBarriers();
    void startAgents();
    void stopAgents();
    void tick(int microseconds);
	void startEntityAgent(EntityPtr entity);
	bool validMove(EntityPtr entity) const;
	void updateEntityLocation(EntityPtr entity);
	void selectBestBrains(std::vector<NeuralNetworkPtr>& preyBrains, std::vector<NeuralNetworkPtr>& predatorBrains);
	Point getBirthLocation(EntityPtr parent);
	InputLayerValues generateEntityPerception(EntityPtr entity);
	void createEntitySaveFile();
	void createNeuralNetworkGraphs();

    static World* m_worldSingletonInstance;
    static std::mutex m_constructorMutex;
    GraphicsHandler* m_graphicsHandler;
    std::vector<EntityPtr> m_entities;
	std::vector<EntityPtr> m_barriers;
	std::vector<SDL_Point> m_barrierPoints;
	std::priority_queue<EntityPtr> m_deadEntities;
	bool m_generationAlive = true;
    std::condition_variable m_cv;
    std::mutex m_mtx;
    std::vector<AgentPtr> m_agents;
    std::atomic<int> m_stopFlag;
    bool m_haltAgents = true;
    bool m_haltAgents2 = true;
};

#endif /* PREYDATOR_WORLD_H__ */