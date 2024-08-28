#include "Entity/entity.h"
#include "graphics_handler.h"
#include "world.h"
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <iostream>

static EntityPtr occupiedSpace[WORLD_X][WORLD_Y] = {nullptr};

World::World(std::vector<EntityPtr> entities, std::vector<EntityPtr> barriers)
    : m_entities(entities), m_barriers(barriers)
{
    m_graphicsHandler = GraphicsHandler::GetInstance(WORLD_X - 1, WORLD_Y - 1);    
}

World::~World()
{
    
}

std::vector<EntityPtr> World::getEntities()
{
    return m_entities;
}


void World::initializeBarriers()
{
	for(auto& barrier : m_barriers)
	{
		barrier->spawn();
		Point location = barrier->getLocation();
		DrawInfo drawInfo = barrier->getDrawInfo();
		for(auto point : drawInfo.points)
		{
			occupiedSpace[location.x + point.x][location.y + point.y] = barrier;
		}
	}
}

void World::startAgents()
{
    m_stopFlag.store(0, std::memory_order_relaxed);
    std::lock_guard<std::mutex> lk(m_mtx);
    for(auto& entity : m_entities)
    {
        entity->setSharedData(&m_cv, &m_mtx, &m_haltAgents, &m_haltAgents2);
		m_agents.push_back({entity, std::thread(&Entity::run, entity, std::ref(m_stopFlag))});
    }
    m_cv.notify_all();
}

void World::stopAgents()
{
    m_stopFlag.store(-1, std::memory_order_relaxed);
    tick();
    for(auto& agent : m_agents)
    {
        agent.thread.join();
    }
}

bool World::alive()
{
    return !m_agents.empty();
}

void World::tick()
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

void World::updateAgents()
{
    std::vector<std::vector<Agent>::iterator> agentsToTerminate;
    for(std::vector<Agent>::iterator itr = m_agents.begin(); itr != m_agents.end(); itr++)
    {
		// If entity wants to move to a valid location
		if(validLocationRequest((*itr).entity))
		{
			// Allow movement
			(*itr).entity->allowLocationRequest();
			updateEntityLocation((*itr).entity);
		}
		// If entity has no healt
        if((*itr).entity->check() & 0x1)
		{
			// kill entity
			(*itr).entity->sendSignal(1);
			//TODO: can a thread even be joinable right after the signal without a tick?
			if((*itr).thread.joinable())
			{
				agentsToTerminate.push_back(itr);
			}
		}
    }
	// Execute one World tick
    tick();
	// Kill and remove all joinable Agents and their threads from last tick
    for(auto agent = agentsToTerminate.rbegin(); agent != agentsToTerminate.rend(); ++agent)
    {
        //std::cout << "join: " << (**agent).entity->getId() << std::endl;
        (**agent).thread.join();
        //std::cout << "erase: " << (**agent).entity->getId() << std::endl;
		m_agents.erase(*agent);
        //std::cout << "done: " << (**agent).entity->getId() << std::endl;
    }
}

void World::drawEntities()
{
	for(auto& barrier : m_barriers)
	{
		m_graphicsHandler->drawEntity(*barrier);
	}
    for(auto& agent : m_agents)
    {
        m_graphicsHandler->drawEntity(*agent.entity);
    }
    m_graphicsHandler->render();
}

bool World::validLocationRequest(EntityPtr entity) const
{
	Point locReq = entity->getLocationRequest();
	if (locReq.x > WORLD_X - 9 || locReq.x < 0 || locReq.y > WORLD_Y - 9 || locReq.y < 0 ) return false;
	for(auto point : entity->getDrawInfo().points)
	{
		if(occupiedSpace[locReq.x + point.x][locReq.y + point.y] && occupiedSpace[locReq.x + point.x][locReq.y + point.y] != entity) return false;
	}
	return true;
}

void World::updateEntityLocation(EntityPtr entity)
{
	Point loc = entity->getLocation();
	Point locReq = entity->getLocationRequest();
	for(auto point : entity->getDrawInfo().points)
	{
		occupiedSpace[loc.x + point.x][loc.y + point.y] = nullptr;
	}
	for(auto point : entity->getDrawInfo().points)
	{
		occupiedSpace[locReq.x + point.x][locReq.y + point.y] = entity;
	}
}