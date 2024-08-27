#include "Entity/entity.h"
#include "Entity/predator.h"
#include "Entity/prey.h"
#include "Entity/plant.h"
#include "Entity/barrier.h"
#include "graphics_handler.h"
#include "world.h"
#include <signal.h>
#include <iostream>
#include <ctime>

static volatile sig_atomic_t quit = false;

void signal_handler(int signum)
{
    quit = true;
}

void usage()
{
    std::cout << "Usage: ./preydator [number]" << std::endl;
    exit(EXIT_FAILURE);
}

int main(int argc, char* argv[])
{
    if(argc != 2) usage();
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    std::srand((unsigned int)std::time(NULL));

    std::vector<EntityPtr> entities;
	std::vector<Barrier> barriers;
    for(int i = 0; i < atoi(argv[1]); i++)
    {
        entities.push_back(std::make_shared<Predator>());
        entities.push_back(std::make_shared<Prey>());
        entities.push_back(std::make_shared<Plant>());
		for(int j = 0; j < 10; j++)
		{
        	barriers.push_back(Barrier());
		}
    }
    World world(entities, barriers);
    world.startAgents();
    while(!quit && world.alive())
    {
        world.updateAgents();
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
