#include "Entity/entity.h"
#include "graphics_handler.h"
#include "world.h"
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <iostream>

static EntityPtr occupiedSpace[WORLD_X][WORLD_Y] = {nullptr};

World* World::m_worldSingletonInstance = nullptr;
std::mutex World::m_constructorMutex;

World* World::GetInstance(std::vector<EntityPtr> entities, std::vector<EntityPtr> barriers)
{
    std::lock_guard<std::mutex> lock(m_constructorMutex);
    if (m_worldSingletonInstance == nullptr)
    {
        m_worldSingletonInstance = new World(entities, barriers);
    }
    return m_worldSingletonInstance;
}

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
		entity->spawn();
		startEntityAgent(entity);
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
	std::vector<EntityPtr> entitiesToStart;
    for(std::vector<Agent>::iterator itr = m_agents.begin(); itr != m_agents.end(); itr++)
    {
		// If entity executes a valid move
		if(validMove((*itr).entity))
		{
			// Allow location update
			(*itr).entity->allowLocationUpdate();
			updateEntityLocation((*itr).entity);
		}
		// If entity can reproduce
		if((*itr).entity->check() & 0x2)
		{
			Point birthLocation = getBirthLocation((*itr).entity);
			if (birthLocation.x >= 0 && birthLocation.y >= 0)
			{
				entitiesToStart.push_back((*itr).entity->giveBirth(birthLocation));
			}
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
	// Start new agents that have been born
	for(auto entity : entitiesToStart)
	{
		m_entities.push_back(entity);
		startEntityAgent(entity);
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

void World::startEntityAgent(EntityPtr entity)
{
	if(!entity) return;
	entity->setSharedData(&m_cv, &m_mtx, &m_haltAgents, &m_haltAgents2);
	Point loc = entity->getLocation();
	for(auto point : entity->getDrawInfo().points)
	{
		occupiedSpace[loc.x + point.x][loc.y + point.y] = entity;
	}
	m_agents.push_back({entity, std::thread(&Entity::run, entity, std::ref(m_stopFlag))});
}

bool World::validMove(EntityPtr entity) const
{
	Point locReq = entity->getLocationRequest();
	if (locReq.x > WORLD_X - 9 || locReq.x < 0 || locReq.y > WORLD_Y - 9 || locReq.y < 0 ) return false;
	for(auto point : entity->getDrawInfo().points)
	{
		EntityPtr occupyingEntity = occupiedSpace[locReq.x + point.x][locReq.y + point.y];
		if(occupyingEntity && occupyingEntity != entity)
		{
			return entity->attack(occupyingEntity);
		}
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

Point World::getBirthLocation(EntityPtr parent)
{
	Point parentLocation = parent->getLocation();
	Point birthLocation = {};
	// Try to find an unoccupied space several times
	for (int i = 0; i < 20; i++)
	{
		// Choose random location around parent
		birthLocation.x = parentLocation.x + (std::rand() % 19) - 9;
		birthLocation.y = parentLocation.y + (std::rand() % 19) - 9;
		// If location is inside the world boundary
		if (birthLocation.x <= WORLD_X - 9 && birthLocation.x >= 0 && birthLocation.y <= WORLD_Y - 9 && birthLocation.y >= 0)
		{
			// Check for every Point of the hitbox
			for(auto point : parent->getDrawInfo().points)
			{
				// If the space is already occupied
				EntityPtr occupyingEntity = occupiedSpace[birthLocation.x + point.x][birthLocation.y + point.y];
				if(occupyingEntity)
				{
					// If so, discard this location
					birthLocation.x = -1;
					birthLocation.y = -1;
					break;
				}
			}
			// If location is valid (for the whole hitbox) return it
			if (birthLocation.x >= 0 && birthLocation.y >= 0) return birthLocation;
		}
	}
	// Return invalid location if no valid location was found
	birthLocation.x = -1;
	birthLocation.y = -1;
	return birthLocation;
}