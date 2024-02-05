#include "entity.h"
#include "graphics_handler.h"
#include "preydator.h"
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>

Preydator::Preydator(std::vector<Entity> entities)
    : m_entities(entities)
{
    m_graphicsHandler = GraphicsHandler::GetInstance(999, 999);    
}

Preydator::~Preydator()
{
    
}

std::vector<Entity> Preydator::getEntities()
{
    return m_entities;
}

void Preydator::startAgents()
{
    m_stopFlag.store(false, std::memory_order_relaxed);
    std::lock_guard<std::mutex> lk(m_mtx);
    for(auto& entity : m_entities)
    {
        entity.setSharedData(&m_cv, &m_mtx, &m_haltAgents, &m_haltAgents2);
        m_agents.push_back(std::make_pair(&entity, std::thread(&Entity::run, &entity, std::ref(m_stopFlag))));
    }
    m_cv.notify_all();
}

void Preydator::stopAgents()
{
    m_stopFlag.store(true, std::memory_order_relaxed);
    tick();
    for(auto& agent : m_agents)
    {
        agent.second.join();
    }
}

void Preydator::tick()
{
    m_haltAgents2 = true;
    m_haltAgents = false;
    m_cv.notify_all();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    m_haltAgents = true;
    m_haltAgents2 = false;
    m_cv.notify_all();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

void Preydator::drawEntities()
{
    for(auto& agent : m_agents)
    {
        m_graphicsHandler->drawEntity(*agent.first);
    }
    m_graphicsHandler->render();
}

