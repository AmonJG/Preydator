#include "entity.h"
#include "graphics_handler.h"
#include "preydator.h"

int main(int argc, char* const* argv) {
    
    std::vector<Entity> entities;
    entities.push_back(Entity(predatorDrawInfo));
    entities.push_back(Entity(preyDrawInfo));
    entities.push_back(Entity(plantDrawInfo));
    entities.push_back(Entity(barrierDrawInfo));

    Preydator world(entities);
    world.startAgents();

    for(int i = 0; i < 720; i = i+1)
    {
        world.tick();
        world.drawEntities();
    }
    world.stopAgents();

    // Start initial Agent Threads
    // Main Loop
    //   Wait for all Agents Actions
    //   Delete and Create Agents and corresponding Threads
    //   Log Agent Informations
    //   Render from atomic 2D-Grid

    return 0;
}
