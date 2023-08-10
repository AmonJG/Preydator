#pragma once
#include "entity.h"
#include <SDL2/SDL.h>

class Preydator
{
public:
    Preydator(std::vector<Entity> entities);
    ~Preydator();
    std::vector<Entity> getEntities();

private:
    std::vector<Entity> m_entities;    

};

