#include "entity.h"
#include "preydator.h"
#include <cstdlib>
#include <iostream>

Entity::Entity(DrawInfo const& drawInfo)
    : m_drawInfo(drawInfo)
{
    m_location.x = std::rand() % 991;
    m_location.y = std::rand() % 991;
}

Entity::Entity(DrawInfo const& drawInfo, Point location)
    : m_drawInfo(drawInfo), m_location(location)
{
    
}

Entity::~Entity()
{
    
}

void Entity::run(std::atomic<bool>& stopFlag)
{
    while(!stopFlag.load(std::memory_order_relaxed))
    {
        std::unique_lock<std::mutex> lk(*mp_mtx);
        while(*mp_haltAgents) mp_cv->wait(lk);
        m_location.x += (std::rand() % 5) - 2;
        m_location.y += (std::rand() % 5) - 2;
        while(*mp_haltAgents2) mp_cv->wait(lk);
    }
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

Point Entity::getLocation() const
{
    return m_location;
}

