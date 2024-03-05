#include "entity.h"
#include "graphics_handler.h"
#include "preydator.h"
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>



#include <iostream>

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
    m_stopFlag.store(0, std::memory_order_relaxed);
    std::lock_guard<std::mutex> lk(m_mtx);
    for(auto& entity : m_entities)
    {
        entity.setSharedData(&m_cv, &m_mtx, &m_haltAgents, &m_haltAgents2);
	m_agents.push_back({&entity, std::thread(&Entity::run, &entity, std::ref(m_stopFlag))});
    }
    m_cv.notify_all();
}

void Preydator::stopAgents()
{
    m_stopFlag.store(-1, std::memory_order_relaxed);
    tick();
    for(auto& agent : m_agents)
    {
        agent.thread.join();
    }
}

bool Preydator::alive()
{
    return !m_agents.empty();
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

void Preydator::updateEntities()
{
    std::vector<std::vector<Agent>::iterator> agentsToTerminate;
    for(std::vector<Agent>::iterator itr = m_agents.begin(); itr != m_agents.end(); itr++)
    {
        if((*itr).entity->check() & 0x1)// && (*itr).entity->getId() == 1)
	{
	    (*itr).entity->sendSignal(1);
	    if((*itr).thread.joinable())
	    {
	        //std::cout << "teminate: " << (*itr).entity->getId() << std::endl;
	        agentsToTerminate.push_back(itr);
	    }
	}
    }
    tick();
    for(auto agent = agentsToTerminate.rbegin(); agent != agentsToTerminate.rend(); ++agent)
    {
        //std::cout << "join: " << (**agent).entity->getId() << std::endl;
        (**agent).thread.join();
        //std::cout << "erase: " << (**agent).entity->getId() << std::endl;
	m_agents.erase(*agent);
        //std::cout << "done: " << (**agent).entity->getId() << std::endl;
    }
}

void Preydator::drawEntities()
{
    for(auto& agent : m_agents)
    {
        m_graphicsHandler->drawEntity(*agent.entity);
    }
    m_graphicsHandler->render();
}

