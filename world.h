#ifndef PREYDATOR_WORLD_H__
#define PREYDATOR_WORLD_H__

#include "preydator_config.h"
#include "Entity/entity.h"
#include "graphics_handler.h"
#include "thread_pool.h"
#include <condition_variable>
#include <mutex>
#include <atomic>
#include <vector>
#include <queue>

struct Agent
{
    Entity& entity;
	Agent(Entity& e) : entity(e) {}
};

using AgentPtr = std::unique_ptr<Agent>;

struct EntityPtrCompare
{
    bool operator()(const EntityPtr& a, const EntityPtr& b) const
    {
        if (a->getFitness() != b->getFitness())
            return a->getFitness() < b->getFitness();
        return a->getGeneration() < b->getGeneration();
    }
};

class World
{
public:
	World(World &other) = delete;
    void operator=(World const&) = delete;
    static World* GetInstance();
    std::vector<EntityPtr>& getEntities();
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
	void initGeneration(std::vector<NeuralNetworkPtr>& preyBrains, std::vector<NeuralNetworkPtr>& predatorBrains);
    void startAgents();
    void stopAgents();
    void tick(int microseconds);
	void startEntityAgent(Entity& entity);
	bool validMove(Entity& entity) const;
	void updateEntityLocation(Entity& entity);
	void freeEntityLocation(Entity& entity);
	void selectBestBrains(int preyAmount, std::vector<NeuralNetworkPtr>& preyBrains, int predatorAmount, std::vector<NeuralNetworkPtr>& predatorBrains);
	Point getBirthLocation(Entity& parent);
	InputLayerValues generateEntityPerception(Entity& entity);
	void createEntitySaveFile();
	void createNeuralNetworkGraphs();
	void createPopulationDataFile();

    static World* m_worldSingletonInstance;
    static std::mutex m_constructorMutex;
    GraphicsHandler* m_graphicsHandler;
    ThreadPool m_pool;
    std::vector<EntityPtr> m_entities;
	std::vector<EntityPtr> m_deadEntities;
	bool m_generationAlive = true;
    std::condition_variable m_cv;
    std::mutex m_mtx;
    std::vector<AgentPtr> m_agents;
    std::atomic<int> m_stopFlag{0};
    bool m_haltAgents = true;
    bool m_haltAgents2 = true;
};

#endif /* PREYDATOR_WORLD_H__ */