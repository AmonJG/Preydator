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
    std::cout << "Usage: ./preydator [files...]" << std::endl;
    std::cout << "INFO: files specifies the neural networks to start the simulation with." << std::endl;
    std::cout << "If no files are specified, all neural networks are randomly generated." << std::endl;
    exit(EXIT_FAILURE);
}

int main(int argc, char* argv[])
{
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    std::srand((unsigned int)std::time(NULL));

	std::vector<std::ifstream> in_files;
	for (int i = 1; i < argc; i++)
	{
		in_files.emplace_back();
		in_files[i-1].open(argv[i]);
		if (!in_files[i-1].is_open())
		{
			std::cerr << "Error opening input file: " << argv[i] << std::endl;
			return -1;
		}
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

    World* world = World::GetInstance();
	if(in_files.empty())
	{
		world->initNewGeneration();
	}
	else
	{
		if (!world->initSavedGeneration(in_files)) return -1;
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
