#include "entity.h"
#include "preydator.h"
#include <cstdlib>
#include <iostream>

static int newId()
{
    static int id = 0;
    return ++id;
}

Entity::Entity(DrawInfo const& drawInfo)
    : m_drawInfo(drawInfo)
{
    m_location.x = std::rand() % 991;
    m_location.y = std::rand() % 991;
    m_id = newId();
    m_health = 100;
}

Entity::Entity(DrawInfo const& drawInfo, Point location)
    : m_drawInfo(drawInfo), m_location(location)
{
    
}

Entity::~Entity()
{
    
}

void Entity::run(std::atomic<int>& stopFlag)
{
    while(stopFlag.load(std::memory_order_relaxed) >= 0 && stopFlag.load(std::memory_order_relaxed) != m_id)
    {
        std::unique_lock<std::mutex> lk(*mp_mtx);
        while(*mp_haltAgents) mp_cv->wait(lk);
        m_location.x += (std::rand() % 5) - 2;
        m_location.y += (std::rand() % 5) - 2;
	m_health -= std::rand() % 3;
        while(*mp_haltAgents2) mp_cv->wait(lk);
    }
    printf("%d terminated!\n", m_id);
}

int Entity::check()
{
    if(m_health <= 0) return 1;
    return 0;
}

void Entity::setSharedData(std::condition_variable* cv, std::mutex* mtx, bool* h1, bool* h2)
{
    mp_cv = cv;
    mp_mtx = mtx;
    mp_haltAgents = h1;
    mp_haltAgents2 = h2;
}

DrawInfo Entity::getDrawInfo() const
{
    return m_drawInfo;
}

int Entity::getId() const
{
    return m_id;
}

Point Entity::getLocation() const
{
    return m_location;
}

