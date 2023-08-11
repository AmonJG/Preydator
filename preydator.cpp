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
    std::lock_guard<std::mutex> lk(m_mtx);
    for(auto& entity : m_entities)
    {
        entity.setSharedData(&m_cv, &m_mtx, &m_haltAgents, &m_haltAgents2);
        m_agents.push_back(std::make_pair(&entity, std::thread(&Entity::run, &entity)));
    }
    m_cv.notify_all();
}

void Preydator::stopAgents()
{
    for(auto& agent : m_agents)
    {
        agent.second.join();
    }
}

void Preydator::tick()
{
    m_haltAgents = false;
    m_cv.notify_all();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    m_haltAgents = true;
    m_haltAgents2 = false;
    m_cv.notify_all();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    m_haltAgents2 = true;
}

void Preydator::drawEntities()
{
    for(auto& entity : m_agents)
    {
        m_graphicsHandler->drawEntity(*entity.first);
    }
    m_graphicsHandler->render();
}

