#include "entity.h"
#include <cstdlib>
#include <iostream>

static int newId()
{
    static int id = 0;
    return ++id;
}
// TODO: remove that all entitites get brains
Entity::Entity()
	: m_brain()
{
    m_id = newId();
    m_health = config.entity_start_health;
    m_reproduction = 0;
    m_signal = 0;
}

Entity::Entity(NeuralNetwork brain)
	: m_brain(/*brain*/)
{
    m_id = newId();
    m_health = config.entity_start_health;
    m_reproduction = 0;
    m_signal = 0;
}

Entity::Entity(NeuralNetwork brain, Point location)
	: m_brain(/*brain*/)
{
    m_id = newId();
    m_health = config.entity_start_health;
    m_reproduction = 0;
    m_signal = 0;
	m_location.x = location.x;
	m_location.y = location.y;
	m_location_request.x = m_location.x;
	m_location_request.y = m_location.y;
}

void Entity::run(std::atomic<int>& stopFlag)
{
    while(stopFlag.load(std::memory_order_relaxed) >= 0 && m_signal == 0)
    {
        std::unique_lock<std::mutex> lk(*mp_mtx);
        while(*mp_haltAgents) mp_cv->wait(lk);
        action();
        while(*mp_haltAgents2) mp_cv->wait(lk);
    }
    //printf("%d terminated!\n", m_id);
}

int Entity::check()
{
    int flags = 0x0;
    if(m_health <= 0) flags |= 0x1;
    if(m_reproduction >= config.entity_reproduction_goal) flags |= 0x2;
    return flags |= m_signal;
}

void Entity::setSharedData(std::condition_variable* cv, std::mutex* mtx, bool* h1, bool* h2)
{
    mp_cv = cv;
    mp_mtx = mtx;
    mp_haltAgents = h1;
    mp_haltAgents2 = h2;
}

void Entity::sendSignal(int signal)
{
    m_signal = signal;
}

int Entity::getId() const
{
    return m_id;
}

Point Entity::getLocation() const
{
    return m_location;
}

Point Entity::getLocationRequest() const
{
    return m_location_request;
}

void Entity::allowLocationUpdate()
{
    m_allow_location_update = true;
}

void Entity::documentSelf()
{
	m_brain.exportGraph();
}