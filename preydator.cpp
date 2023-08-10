#include "entity.h"
#include "preydator.h"

Preydator::Preydator(std::vector<Entity> entities)
    : m_entities(entities)
{
    
}

Preydator::~Preydator()
{
    
}

std::vector<Entity> Preydator::getEntities()
{
    return m_entities;
}

