#ifndef PREYDATOR_PREYDATOR_H__
#define PREYDATOR_PREYDATOR_H__

#include "Entity/entity.h"
#include "graphics_handler.h"
#include <condition_variable>
#include <mutex>
#include <atomic>
#include <thread>
#include <vector>

typedef struct {
    std::shared_ptr<Entity> entity;
    std::thread thread;
} Agent;

class Preydator
{
public:
    Preydator(std::vector<std::shared_ptr<Entity>> entities);
    ~Preydator();
    std::vector<std::shared_ptr<Entity>> getEntities();
    void startAgents();
    void stopAgents();
    bool alive();
    void tick();
    void updateEntities();
    void drawEntities();

private:
    GraphicsHandler* m_graphicsHandler;  
    std::vector<std::shared_ptr<Entity>> m_entities;
    std::condition_variable m_cv;
    std::mutex m_mtx;
    std::vector<Agent> m_agents;
    std::atomic<int> m_stopFlag;
    bool m_haltAgents = true;
    bool m_haltAgents2 = true;
};

#endif /* PREYDATOR_PREYDATOR_H__ */