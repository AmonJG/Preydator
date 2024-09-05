#include "preydator_config.h"
#include "Entity/entity.h"
#include "Entity/predator.h"
#include "Entity/prey.h"
#include "Entity/plant.h"
#include "Entity/barrier.h"
#include "graphics_handler.h"
#include "world.h"
#include <signal.h>
#include <iostream>
#include <fstream>
#include <ctime>

static volatile sig_atomic_t quit = false;
static std::string config_file_name = "preydator.config";
static std::ifstream config_file;
PreydatorConfig config;

void signal_handler(int signum)
{
    quit = true;
}

void usage()
{
    std::cout << "Usage: ./preydator" << std::endl;
    exit(EXIT_FAILURE);
}

int main(int argc, char* argv[])
{
    if(argc != 1) usage();
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    std::srand((unsigned int)std::time(NULL));

	config_file.open(config_file_name);
	if (!config_file.is_open())
	{
        std::cerr << "Error opening config file: " << config_file_name << std::endl;
		return -1;
    }

	for (std::string line; std::getline(config_file, line);)
    {
		size_t pos = line.find('=');
		if (pos != std::string::npos)
		{
			std::string key = line.substr(0, pos);
			std::string value = line.substr(pos + 1);
			preydator_config_assign_map.at(key)(value);
		}
	}

    World* world = World::GetInstance();
	world->initializeBarriers();
    world->startAgents();
    while(!quit && world->alive())
    {
        world->updateAgents();
        world->drawEntities();
    }
    world->stopAgents();

    // Start initial Agent Threads
    // Main Loop
    //   Wait for all Agents Actions
    //   Delete and Create Agents and corresponding Threads
    //   Log Agent Informations
    //   Render from atomic 2D-Grid

    return 0;
}
