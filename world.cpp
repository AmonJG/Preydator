#include "Entity/entity.h"
#include "Entity/plant.h"
#include "Entity/prey.h"
#include "Entity/predator.h"
#include "graphics_handler.h"
#include "preydator_math.h"
#include "world.h"
#include <condition_variable>
#include <algorithm>
#include <mutex>
#include <thread>
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

static EntityPtr occupiedSpace[WORLD_X][WORLD_Y] = {nullptr};
static unsigned int tickCounter = 0;
static unsigned int generationCounter = 0;
static unsigned int peakPlantPopulation = 0;
static unsigned int peakPreyPopulation = 0;
static unsigned int peakPredatorPopulation = 0;
std::ofstream populationDataFile;
std::ofstream logFile;
std::string dataPath;

static bool parseError(std::string reason)
{
	std::cerr << "Error parsing input file" << std::endl;
	std::cerr << "Reason: " << reason << std::endl;
	return false;
}

World* World::m_worldSingletonInstance = nullptr;
std::mutex World::m_constructorMutex;

World* World::GetInstance()
{
    std::lock_guard<std::mutex> lock(m_constructorMutex);
    if (m_worldSingletonInstance == nullptr)
    {
        m_worldSingletonInstance = new World();
    }
    return m_worldSingletonInstance;
}

World::World()
	: m_pool(std::thread::hardware_concurrency())
{
	if (config.show_animation)
	{
		m_graphicsHandler = GraphicsHandler::GetInstance(WORLD_X - 1, WORLD_Y - 1);
	}

	std::string timestamp = getCurrentTimestamp();
	if (config.show_animation)
	{
		dataPath = "data/exec_animated_" + timestamp;
	}
	else
	{
		dataPath = "data/exec_" + timestamp;
	}
	std::filesystem::create_directory(dataPath);
	dataPath += "/";

	std::string filename = dataPath + "preydator_" + timestamp + ".log";
	logFile.open(filename);

	if (!logFile.is_open())
	{
        std::cerr << "Error opening log file!" << std::endl;
    }
}

World::~World()
{
    populationDataFile.close();
	logFile.close();
}

std::vector<EntityPtr> World::getEntities()
{
    return m_entities;
}

void World::initNewGeneration()
{
	std::vector<NeuralNetworkPtr> preyBrains;
	std::vector<NeuralNetworkPtr> predatorBrains;
	selectBestBrains(preyBrains, predatorBrains);
	initGeneration(preyBrains, predatorBrains);
}

bool World::initSavedGeneration(std::string const& in_file_path)
{
	logFile << "[INFO] Execution initialized with input file: " << in_file_path << std::endl;
	std::vector<NeuralNetworkPtr> preyBrains;
	std::vector<NeuralNetworkPtr> predatorBrains;
	std::ifstream in_file;
	in_file.open(in_file_path);
	if (!in_file.is_open())
	{
		std::cerr << "Error opening input file: " << in_file_path << std::endl;
		return false;
	}

	std::string line;
	std::getline(in_file, line);

	try
	{
		generationCounter = std::stoi(line);
	}
	catch (...)
	{
		std::cerr << "Error: First line of data file is not a number (of the generation)." << std::endl;
		return false;
	}

	bool isPreyBrain = false;
	while (std::getline(in_file, line))
	{
		if(line == "___Prey___")
		{
			isPreyBrain = true;
		}
		else if(line == "___Predator___")
		{
			isPreyBrain = false;
		}
		else
		{
			return parseError("No entity type defined in input file");
		}
		NeuralNetworkPtr brain = std::make_shared<NeuralNetwork>(in_file);
		isPreyBrain ? preyBrains.push_back(brain) : predatorBrains.push_back(brain);
	}

	if (preyBrains.size() > config.prey_start_amount)
	{
		std::cerr << "WARNING: Prey amount truncated from "
			<< preyBrains.size() << " to config defined prey_start_amount "
			<< config.prey_start_amount << std::endl;
		preyBrains.resize(config.prey_start_amount);
	}
	if (preyBrains.size() < config.prey_start_amount)
	{
		std::cerr << "WARNING: Prey amount increased from "
			<< preyBrains.size() << " to config defined prey_start_amount "
			<< config.prey_start_amount << " with random brains" << std::endl;
		while(preyBrains.size() < config.prey_start_amount)
		{
			preyBrains.emplace_back(std::make_shared<NeuralNetwork>());
		}
	}
	if (predatorBrains.size() > config.predators_start_amount)
	{
		std::cerr << "WARNING: Predator amount truncated from "
			<< predatorBrains.size() << " to config defined predators_start_amount "
			<< config.predators_start_amount << std::endl;
		predatorBrains.resize(config.predators_start_amount);
	}
	if (predatorBrains.size() < config.predators_start_amount)
	{
		std::cerr << "WARNING: Predator amount increased from "
			<< predatorBrains.size() << " to config defined predators_start_amount "
			<< config.predators_start_amount << " with random brains" << std::endl;
		while(predatorBrains.size() < config.predators_start_amount)
		{
			predatorBrains.emplace_back(std::make_shared<NeuralNetwork>());
		}
	}

	initGeneration(preyBrains, predatorBrains);
	in_file.close();
	return true;
}

void World::killGeneration()
{
	for (auto entity : m_entities)
	{
		if (!std::dynamic_pointer_cast<Plant>(entity))
		{
			m_deadEntities.push(entity);
		}
	}
	logFile << "[INFO] Killing remaining " << m_agents.size() << " agents." << std::endl;
	stopAgents();
	m_agents.clear();
	m_entities.clear();
	m_barriers.clear();

	for (int i = 0; i < WORLD_X; ++i)
	{
        for (int j = 0; j < WORLD_Y; ++j)
		{
            occupiedSpace[i][j] = nullptr;
        }
    }
	m_barrierPoints.clear();
	m_pool.clear(); //TODO: check if new initialization is better
	tickCounter = 0;
	peakPlantPopulation = 0;
	peakPreyPopulation = 0;
	peakPredatorPopulation = 0;
}

bool World::generationAlive()
{
	return m_generationAlive;
}

void World::updateAgents()
{
    std::vector<AgentPtr> agentsToTerminate;
    std::vector<EntityPtr> entitiesToStart;

    unsigned int plantPopulation = 0;
    unsigned int preyPopulation = 0;
    unsigned int predatorPopulation = 0;

    for (auto& agent : m_agents)
    {
        auto& entity = agent->entity;

		// TODO change to altering population instead of countig it every tick
        if (std::dynamic_pointer_cast<Plant>(entity)) plantPopulation++;
        if (std::dynamic_pointer_cast<Prey>(entity)) preyPopulation++;
        if (std::dynamic_pointer_cast<Predator>(entity)) predatorPopulation++;

        // If entity executes a valid move
        if (validMove(entity))
        {
			// Allow location update
            entity->allowLocationUpdate();
            updateEntityLocation(entity);
        }
        // If entity can reproduce
        if (entity->check() & 0x2)
        {
            Point birthLocation = getBirthLocation(entity);
            if (birthLocation.x >= 0 && birthLocation.y >= 0)
            {
                entitiesToStart.push_back(
                    entity->giveBirth(birthLocation)
                );
            }
        }
        // If entity has no health
        if (entity->check() & 0x1)
        {
			// Kill entity
            entity->sendSignal(1);
            agentsToTerminate.push_back(agent);
            continue;
        }
        // Send perception data to brain (input layer of neural network)
        entity->perceive(generateEntityPerception(entity));
    }

	peakPlantPopulation = std::max(plantPopulation, peakPlantPopulation);
	peakPreyPopulation = std::max(preyPopulation, peakPreyPopulation);
	peakPredatorPopulation = std::max(predatorPopulation, peakPredatorPopulation);

	if (populationDataFile.is_open())
		populationDataFile << tickCounter << "," << predatorPopulation << ","
			<< preyPopulation << "," << plantPopulation << "\n";

    if (predatorPopulation == 0 || preyPopulation == 0)
        m_generationAlive = false;

	// Execute one World tick
    tick(config.tick_delay);

    if (tickCounter >= config.max_ticks_per_generation)
        m_generationAlive = false;

    // Remove dead Agents
    for (auto& agent : agentsToTerminate)
    {
        if (!std::dynamic_pointer_cast<Plant>(agent->entity))
        {
            m_deadEntities.push(agent->entity);
        }
        m_agents.erase(
            std::remove(m_agents.begin(), m_agents.end(), agent),
            m_agents.end()
        );
    }

    // Start Newborns
    for (auto& entity : entitiesToStart)
    {
        m_entities.push_back(entity);
        startEntityAgent(entity);
    }

	if (!m_generationAlive)
	{
		logFile << "[INFO] Generation ended at tick: " << tickCounter << "/"
			<< config.max_ticks_per_generation << std::endl;
		logFile << "[INFO] Current population: " << std::endl;
		logFile << "       Plants:    " << plantPopulation << std::endl;
		logFile << "       Prey:      " << preyPopulation << std::endl;
		logFile << "       Predators: " << predatorPopulation << std::endl;
		logFile << "[INFO] Population peaks: " << std::endl;
		logFile << "       Plants:    " << peakPlantPopulation << std::endl;
		logFile << "       Prey:      " << peakPreyPopulation << std::endl;
		logFile << "       Predators: " << peakPredatorPopulation << std::endl;
	}

}

void World::drawEntities()
{
	if (!config.show_animation) return;
	m_graphicsHandler->drawPoints(m_barrierPoints, {128, 128, 128, 255});
    for(auto agent : m_agents)
    {
        m_graphicsHandler->drawEntity(*(agent->entity));
    }
    m_graphicsHandler->render();
}

void World::initGeneration(std::vector<NeuralNetworkPtr> const& preyBrains, std::vector<NeuralNetworkPtr> const& predatorBrains)
{
	std::string currentTimestamp = getCurrentTimestamp();
	std::cout << currentTimestamp << " Generation: " << ++generationCounter << std::endl;
	logFile << std::endl << currentTimestamp << " Generation: " << generationCounter << std::endl;
	m_generationAlive = true;

    for(unsigned int i = 0; i < config.barriers_start_amount; i++)
    {
        m_barriers.push_back(std::make_shared<Barrier>());
    }
	for(unsigned int i = 0; i < config.plants_start_amount; i++)
    {
        m_entities.push_back(std::make_shared<Plant>());
    }
	for(unsigned int i = 0; i < config.prey_start_amount; i++)
    {
        m_entities.push_back(std::make_shared<Prey>(preyBrains[i]));
    }
	for(unsigned int i = 0; i < config.predators_start_amount; i++)
    {
        m_entities.push_back(std::make_shared<Predator>(predatorBrains[i]));
    }
	initializeBarriers();
    startAgents();
}

void World::initializeBarriers()
{
	for(auto& barrier : m_barriers)
	{
		barrier->spawn();
		Point location = barrier->getLocation();
		DrawInfo drawInfo = barrier->getDrawInfo();
		for(auto point : drawInfo.points)
		{
			occupiedSpace[location.x + point.x][location.y + point.y] = barrier;
			m_barrierPoints.push_back({location.x + point.x, location.y + point.y});
		}
	}
}

void World::startAgents()
{
    m_stopFlag.store(0, std::memory_order_release);
	if (generationCounter % 100 == 0)
	{
		createPopulationDataFile();
	}
	for(auto& entity : m_entities)
	{
		entity->spawn();
		startEntityAgent(entity);
	}
	logFile << "[INFO] All " << m_agents.size()
		<< " agents have been started for current generation." << std::endl;
}

void World::stopAgents()
{
    m_stopFlag.store(-1, std::memory_order_release);
	if (generationCounter % 100 == 0)
	{
		createEntitySaveFile();
		createNeuralNetworkGraphs();
	}
	populationDataFile.close();
    std::cout << std::endl;
	logFile << "[INFO] All " << m_agents.size()
		<< " agents have been stopped for current generation." << std::endl;
}

void World::tick(int microseconds)
{
	std::cout << "\rTick: " << ++tickCounter << "/" << config.max_ticks_per_generation << std::flush;

    for (auto& agent : m_agents)
	{
        m_pool.enqueue([entity = agent->entity, &stop = m_stopFlag] {
            if (stop.load(std::memory_order_acquire) >= 0) entity->tick();
        });
    }
    m_pool.wait();
	std::this_thread::sleep_for(std::chrono::microseconds(microseconds));
}

void World::startEntityAgent(EntityPtr entity)
{
    if (!entity) return;

    Point loc = entity->getLocation();
    for (auto point : entity->getDrawInfo().points)
    {
        occupiedSpace[loc.x + point.x][loc.y + point.y] = entity;
    }

    m_agents.push_back(std::make_shared<Agent>(Agent{entity}));
}

bool World::validMove(EntityPtr entity) const
{
	Point locReq = entity->getLocationRequest();
	if (locReq.x > WORLD_X - 9 || locReq.x < 0 || locReq.y > WORLD_Y - 9 || locReq.y < 0 ) return false;
	for(auto point : entity->getDrawInfo().points)
	{
		EntityPtr occupyingEntity = occupiedSpace[locReq.x + point.x][locReq.y + point.y];
		if(occupyingEntity && occupyingEntity != entity)
		{
			return entity->attack(occupyingEntity);
		}
	}
	return true;
}

void World::updateEntityLocation(EntityPtr entity)
{
	Point loc = entity->getLocation();
	Point locReq = entity->getLocationRequest();
	for(auto point : entity->getDrawInfo().points)
	{
		occupiedSpace[loc.x + point.x][loc.y + point.y] = nullptr;
	}
	for(auto point : entity->getDrawInfo().points)
	{
		occupiedSpace[locReq.x + point.x][locReq.y + point.y] = entity;
	}
}


void World::selectBestBrains(std::vector<NeuralNetworkPtr>& preyBrains, std::vector<NeuralNetworkPtr>& predatorBrains)
{
	int preyAmount = config.prey_start_amount;
	int predatorAmount = config.predators_start_amount;
	logFile << "[INFO] Selecting best brains from " << m_deadEntities.size() << " dead entities." << std::endl;
	while (preyAmount > 0 || predatorAmount > 0)
	{
		if (m_deadEntities.empty())
		{
			logFile << "[INFO] No dead entities left to select brains from." << std::endl;
			logFile << "[INFO] Generating random neural networks for " << preyAmount << " preys and "
				<< predatorAmount << " predators." << std::endl;
			while (preyAmount-- > 0) preyBrains.emplace_back(std::make_shared<NeuralNetwork>());
			while (predatorAmount-- > 0) predatorBrains.emplace_back(std::make_shared<NeuralNetwork>());
			return;
		}
		// Top brain for next generation
		EntityPtr deadEntity = m_deadEntities.top();
		int offsping_amount = generateRandomInt(1, 3);

		// No offspring with 5% chance and insert random brain instead
		if (trueWithProb(0.05))
		{
			std::dynamic_pointer_cast<Prey>(deadEntity)
			? preyBrains.emplace_back(std::make_shared<NeuralNetwork>())
			: predatorBrains.emplace_back(std::make_shared<NeuralNetwork>());
		}
		else if (preyAmount > 0 && std::dynamic_pointer_cast<Prey>(deadEntity))
		{
			while (offsping_amount-- > 0 && preyAmount-- > 0)
			{
				preyBrains.push_back(std::make_shared<NeuralNetwork>(*(deadEntity->getBrain())));
			}
		}
		else if (predatorAmount > 0 && std::dynamic_pointer_cast<Predator>(deadEntity))
		{
			while (offsping_amount-- > 0 && predatorAmount-- > 0)
			{
				predatorBrains.push_back(std::make_shared<NeuralNetwork>(*(deadEntity->getBrain())));
			}
		}
		m_deadEntities.pop();
	}
	while (!m_deadEntities.empty()) m_deadEntities.pop();
}

Point World::getBirthLocation(EntityPtr parent)
{
	Point parentLocation = parent->getLocation();
	Point birthLocation = {};
	// Try to find an unoccupied space several times
	for (int i = 0; i < 20; i++)
	{
		// Choose random location around parent
		birthLocation.x = parentLocation.x + (std::rand() % 19) - 9;
		birthLocation.y = parentLocation.y + (std::rand() % 19) - 9;
		// If location is inside the world boundary
		if (birthLocation.x <= WORLD_X - 9 && birthLocation.x >= 0 && birthLocation.y <= WORLD_Y - 9 && birthLocation.y >= 0)
		{
			// Check for every Point of the hitbox
			for(auto point : parent->getDrawInfo().points)
			{
				// If the space is already occupied
				EntityPtr occupyingEntity = occupiedSpace[birthLocation.x + point.x][birthLocation.y + point.y];
				if(occupyingEntity)
				{
					// If so, discard this location
					birthLocation.x = -1;
					birthLocation.y = -1;
					break;
				}
			}
			// If location is valid (for the whole hitbox) return it
			if (birthLocation.x >= 0 && birthLocation.y >= 0) return birthLocation;
		}
	}
	// Return invalid location if no valid location was found
	birthLocation.x = -1;
	birthLocation.y = -1;
	return birthLocation;
}

InputLayerValues World::generateEntityPerception(EntityPtr entity)
{
	if(std::dynamic_pointer_cast<Plant>(entity)) return InputLayerValues{};
	InputLayerValues input_layer_values;
	Point entityLocation = entity->getLocation();
	for (auto perception_mapping : perception_mapping_matrix)
	{
		int x = perception_mapping.point.x + entityLocation.x;
		int y = perception_mapping.point.y + entityLocation.y;
		if (x < WORLD_X && x >= 0 && y < WORLD_Y && y >= 0)
		{
			EntityPtr occupyingEntity = occupiedSpace[x][y];
			if(occupyingEntity)
			{
				input_layer_values.push_back({perception_mapping.node_id, occupyingEntity->getPerceptionValue(entity)});
				//std::cout << input_layer_values.back().id << " " << input_layer_values.back().value << std::endl;
			}
			else
			{
				input_layer_values.push_back({perception_mapping.node_id, EMPTY_INPUT_LAYER_VALUE});
			}
		}
		else
		{
			input_layer_values.push_back({perception_mapping.node_id, INVALID_INPUT_LAYER_VALUE});
		}
	}
	return input_layer_values;
}

void World::createEntitySaveFile()
{
	std::ofstream out_file;
	std::string timestamp = getCurrentTimestamp();
	std::string filename = dataPath + "entitySaveFile_Gen_" + std::to_string(generationCounter)
		+ "_" + timestamp + ".dat";
	out_file.open(filename);

    if (!out_file.is_open())
	{
        std::cerr << "Error opening entity save file!" << std::endl;
		return;
    }
	out_file << std::to_string(generationCounter) << std::endl;
	for (auto entity : m_entities)
	{
		out_file << entity->createSaveString();
	}
	out_file.close();
}

void World::createNeuralNetworkGraphs()
{
	std::ofstream out_file;
	std::string timestamp = getCurrentTimestamp();
	std::string filename = dataPath + "neuralNetworkGraphs_Gen_" + std::to_string(generationCounter)
		+ "_" + timestamp + ".dot";
	out_file.open(filename);

	if (!out_file.is_open())
	{
		std::cerr << "Error opening neural network graph export file!" << std::endl;
		return;
	}
	for (auto entity : m_entities)
	{
		out_file << entity->documentSelf();
	}
	out_file.close();
}

void World::createPopulationDataFile()
{
	std::string timestamp = getCurrentTimestamp();
	std::string filename = dataPath + "populationData_Gen_" + std::to_string(generationCounter)
		+ "_" + timestamp + ".csv";
	populationDataFile.open(filename);

	if (!populationDataFile.is_open())
	{
        std::cerr << "Error opening population data file!" << std::endl;
		return;
    }
	populationDataFile << "Tick,Predators,Preys,Plants" << std::endl;
}
