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
    std::cout << "Usage: ./preydator [file]" << std::endl;
    std::cout << "INFO: file specifies the neural networks to start the simulation with." << std::endl;
    std::cout << "If no file is specified, all neural networks are randomly generated." << std::endl;
    exit(EXIT_FAILURE);
}

int main(int argc, char* argv[])
{
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    std::srand((unsigned int)std::time(NULL));

	std::string in_file_path;
	if (argc == 2)
	{
		in_file_path = argv[1];
	}

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
	if (config.init_hidden_layers < 1)
	{
		std::cerr << "Error: There has to be at least one initial hidden layer!" << std::endl;
		return -1;
	}

    World* world = World::GetInstance();
	if(in_file_path.empty())
	{
		world->initNewGeneration();
	}
	else
	{
		if (!world->initSavedGeneration(in_file_path)) return -1;
	}

    while (true)
    {
		while (!quit && world->generationAlive())
		{
			world->updateAgents();
			world->drawEntities();
		}
		world->killGeneration();
		if (quit) break;
		world->initNewGeneration();
    }

    // Start initial Agent Threads
    // Main Loop
    //   Wait for all Agents Actions
    //   Delete and Create Agents and corresponding Threads
    //   Log Agent Informations
    //   Render from atomic 2D-Grid

    return 0;
}
