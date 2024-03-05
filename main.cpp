#include "entity.h"
#include "graphics_handler.h"
#include "preydator.h"
#include <signal.h>

static volatile sig_atomic_t quit = false;

void signal_handler(int signum)
{
    quit = true;
}

int main(int argc, char* argv[])
{
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    std::vector<Entity> entities;
    entities.push_back(Entity(predatorDrawInfo));
    entities.push_back(Entity(preyDrawInfo));
    entities.push_back(Entity(plantDrawInfo));
    entities.push_back(Entity(barrierDrawInfo));

    Preydator world(entities);
    world.startAgents();
    while(!quit)
    {
	world.updateEntities();
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
