#pragma once
#include "entity.h"
#include "graphics_handler.h"
#include <condition_variable>
#include <mutex>
#include <atomic>
#include <thread>
#include <vector>

typedef struct {
    Entity* entity;
    std::thread thread;
} Agent;

class Preydator
{
public:
    Preydator(std::vector<Entity> entities);
    ~Preydator();
    std::vector<Entity> getEntities();
    void startAgents();
    void stopAgents();
    void tick();
    void checkEntities();
    void drawEntities();

private:
    GraphicsHandler* m_graphicsHandler;  
    std::vector<Entity> m_entities;
    std::condition_variable m_cv;
    std::mutex m_mtx;
    std::vector<Agent> m_agents;
    std::atomic<int> m_stopFlag;
    bool m_haltAgents = true;
    bool m_haltAgents2 = true;
};

