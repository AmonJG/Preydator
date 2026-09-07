#include "Entity/entity.h"
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

static Entity* occupiedSpace[WORLD_X][WORLD_Y] = {nullptr};
static unsigned int tickCounter = 0;
static unsigned int generationCounter = 0;
static unsigned int peakPreyPopulation = 0;
static unsigned int peakPredatorPopulation = 0;
std::ofstream populationDataFile;
std::ofstream logFile;
std::string dataPath;
std::string populationDataPath;

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
	populationDataPath = dataPath + "population_data";
	std::filesystem::create_directory(populationDataPath);
	populationDataPath += "/";

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

std::vector<EntityPtr>& World::getEntities()
{
    return m_entities;
}

void World::initNewGeneration()
{
	std::vector<NeuralNetworkPtr> preyBrains;
	std::vector<NeuralNetworkPtr> predatorBrains;
	selectBestBrains(config.prey_start_amount, preyBrains, config.predators_start_amount, predatorBrains);
	m_deadEntities.clear();
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
		isPreyBrain
			? preyBrains.push_back(std::make_unique<NeuralNetwork>(in_file))
			: predatorBrains.push_back(std::make_unique<NeuralNetwork>(in_file));
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
			preyBrains.emplace_back(std::make_unique<NeuralNetwork>());
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
			predatorBrains.emplace_back(std::make_unique<NeuralNetwork>());
		}
	}

	initGeneration(preyBrains, predatorBrains);
	in_file.close();
	return true;
}

void World::killGeneration()
{
	logFile << "[INFO] Killing remaining " << m_agents.size() << " agents." << std::endl;
	stopAgents();

	for (auto& entity : m_entities)
	{
		m_deadEntities.push_back(std::move(entity));
	}
	std::sort(m_deadEntities.begin(), m_deadEntities.end(), EntityPtrCompare{});

	m_agents.clear();
	m_entities.clear();

	for (int i = 0; i < WORLD_X; ++i)
	{
        for (int j = 0; j < WORLD_Y; ++j)
		{
            occupiedSpace[i][j] = nullptr;
        }
    }
	m_pool.clear(); //TODO: check if new initialization is better
	tickCounter = 0;
	peakPreyPopulation = 0;
	peakPredatorPopulation = 0;
}

bool World::generationAlive()
{
	return m_generationAlive;
}

void World::updateAgents()
{
    std::vector<Agent*> agentsToTerminate;
    std::vector<Entity*> entitiesToStart;

    unsigned int preyPopulation = 0;
    unsigned int predatorPopulation = 0;

    for (auto& agent : m_agents)
    {
        Entity& entity = agent->entity;

		// TODO change to altering population instead of countig it every tick
        if (dynamic_cast<Prey*>(&entity)) preyPopulation++;
        if (dynamic_cast<Predator*>(&entity)) predatorPopulation++;

        // If entity executes a valid move
        if (validMove(entity))
        {
			// Allow location update
            entity.allowLocationUpdate();
            updateEntityLocation(entity);
        }
        // If entity can reproduce
        if (entity.check() & 0x2)
        {
            Point birthLocation = getBirthLocation(entity);
            if (birthLocation.x >= 0 && birthLocation.y >= 0)
            {
				EntityPtr child = entity.giveBirth(birthLocation);
                entitiesToStart.push_back(child.get());
				m_entities.push_back(std::move(child));
            }
        }
        // If entity has no health
        if (entity.check() & 0x1)
        {
			// Kill entity
            entity.sendSignal(1);
            agentsToTerminate.push_back(agent.get());
            continue;
        }
        // Send perception data to brain (input layer of neural network)
        entity.perceive(generateEntityPerception(entity));
    }

	peakPreyPopulation = std::max(preyPopulation, peakPreyPopulation);
	peakPredatorPopulation = std::max(predatorPopulation, peakPredatorPopulation);

	if (populationDataFile.is_open())// && !config.training_mode)
		populationDataFile << tickCounter << "," << predatorPopulation << ","
			<< preyPopulation << "\n" << std::flush;

    if (predatorPopulation == 0 || preyPopulation == 0)
        m_generationAlive = false;

	// Execute one World tick
    tick(config.tick_delay);

    if (!config.training_mode && tickCounter >= config.max_ticks_per_generation)
        m_generationAlive = false;

    // Remove dead Agents
    for (Agent* agent : agentsToTerminate)
    {
		Entity* entity = &agent->entity;
		freeEntityLocation(*entity);
		auto it = std::find_if(
			m_entities.begin(),
			m_entities.end(),
			[entity](const std::unique_ptr<Entity>& e){ return e.get() == entity; }
		);

		if (it != m_entities.end())
		{
			m_deadEntities.push_back(std::move(*it));
			m_entities.erase(it);
		}
		m_agents.erase(
			std::remove_if(
				m_agents.begin(),
				m_agents.end(),
				[agent](const std::unique_ptr<Agent>& a) { return a.get() == agent; }
			),
			m_agents.end()
		);
    }

    // Start Newborns
    for (Entity* entity : entitiesToStart)
    {
        startEntityAgent(*entity);
    }

	if (!m_generationAlive)
	{
		logFile << "[INFO] Generation ended at tick: " << tickCounter << "/"
			<< config.max_ticks_per_generation << std::endl;
		logFile << "[INFO] Current population: " << std::endl;
		logFile << "       Prey:      " << preyPopulation << std::endl;
		logFile << "       Predators: " << predatorPopulation << std::endl;
		logFile << "[INFO] Population peaks: " << std::endl;
		logFile << "       Prey:      " << peakPreyPopulation << std::endl;
		logFile << "       Predators: " << peakPredatorPopulation << std::endl;
	}

}

void World::updateAgentsTraining()
{
    std::vector<Agent*> agentsToTerminate;
    std::vector<Entity*> entitiesToStart;

    unsigned int preyPopulation = 0;
    unsigned int predatorPopulation = 0;

    for (auto& agent : m_agents)
    {
        Entity& entity = agent->entity;

		// TODO change to altering population instead of countig it every tick
        if (dynamic_cast<Prey*>(&entity)) preyPopulation++;
        if (dynamic_cast<Predator*>(&entity)) predatorPopulation++;

        // If entity executes a valid move
        if (validMove(entity))
        {
			// Allow location update
            entity.allowLocationUpdate();
            updateEntityLocation(entity);
        }
        // If entity can reproduce
        if (entity.check() & 0x2)
        {
            Point birthLocation = getBirthLocation(entity);
            if (birthLocation.x >= 0 && birthLocation.y >= 0)
            {
				EntityPtr child = entity.giveBirth(birthLocation);
                entitiesToStart.push_back(child.get());
				m_entities.push_back(std::move(child));
            }
        }
        // If entity has no health
        if (entity.check() & 0x1)
        {
			// Kill entity
            entity.sendSignal(1);
            agentsToTerminate.push_back(agent.get());
            continue;
        }
        // Send perception data to brain (input layer of neural network)
        entity.perceive(generateEntityPerception(entity));
    }
	
    if (tickCounter > config.max_ticks_per_training_generation)
        m_generationAlive = false;

	// Execute one World tick
    tick(config.tick_delay);

    // Remove dead Agents
    for (Agent* agent : agentsToTerminate)
    {
		Entity* entity = &agent->entity;
		freeEntityLocation(*entity);
		auto it = std::find_if(
			m_entities.begin(),
			m_entities.end(),
			[entity](const std::unique_ptr<Entity>& e){ return e.get() == entity; }
		);

		if (it != m_entities.end())
		{
			m_deadEntities.push_back(std::move(*it));
			m_entities.erase(it);
		}
		m_agents.erase(
			std::remove_if(
				m_agents.begin(),
				m_agents.end(),
				[agent](const std::unique_ptr<Agent>& a) { return a.get() == agent; }
			),
			m_agents.end()
		);
    }

    // Start Newborns
    for (Entity* entity : entitiesToStart)
    {
        startEntityAgent(*entity);
	}
}

void World::drawEntities()
{
	if (!config.show_animation) return;
    for(auto& agent : m_agents)
    {
        m_graphicsHandler->drawEntity(agent->entity);
    }
    m_graphicsHandler->render();
}

void World::initGeneration(std::vector<NeuralNetworkPtr>& preyBrains, std::vector<NeuralNetworkPtr>& predatorBrains)
{
	std::string currentTimestamp = getCurrentTimestamp();
	std::cout << currentTimestamp << " Generation: " << ++generationCounter << std::endl;
	logFile << std::endl << currentTimestamp << " Generation: " << generationCounter << std::endl;
	m_generationAlive = true;

	for(unsigned int i = 0; i < config.prey_start_amount; i++)
    {
        m_entities.push_back(std::make_unique<Prey>(std::move(preyBrains[i]), generationCounter));
    }
	for(unsigned int i = 0; i < config.predators_start_amount; i++)
    {
        m_entities.push_back(std::make_unique<Predator>(std::move(predatorBrains[i]), generationCounter));
    }
    config.training_mode ? startAgentsTraining() : startAgents();
}

void World::startAgents()
{
    m_stopFlag.store(0, std::memory_order_release);
	createPopulationDataFile();
	//if (generationCounter % 100 == 0)// || tickCounter >= 1000)
	{
		createEntitySaveFile();
		createNeuralNetworkGraphs();
	}
	for(auto& entity : m_entities)
	{
		entity->spawn();
		startEntityAgent(*entity);
	}
	logFile << "[INFO] All " << m_agents.size()
		<< " agents have been started for current generation." << std::endl;
}

void World::startAgentsTraining()
{
    m_stopFlag.store(0, std::memory_order_release);
	if (generationCounter % 5000 == 0)
	{
		createEntitySaveFile();
		createNeuralNetworkGraphs();
	}

	size_t entityIndex = 0;
	for (int y = 100; y <= WORLD_Y-100 && entityIndex < m_entities.size(); y += 100)
	{
		for (int x = 100; x <= WORLD_X-100 && entityIndex < m_entities.size(); x += 100)
		{
			while(entityIndex < m_entities.size() && !dynamic_cast<Predator*>(m_entities[entityIndex].get()))
				++entityIndex;
			if(!m_entities[entityIndex].get()) continue;
			m_entities[entityIndex]->spawn(x, y);
			startEntityAgent(*m_entities[entityIndex]);
			++entityIndex;
		}
	}
	entityIndex = 0;
	for (int y = 100; y <= WORLD_Y-100 && entityIndex < m_entities.size(); y += 100)
	{
		for (int x = 100; x <= WORLD_X-100 && entityIndex < m_entities.size(); x += 100)
		{
			while(entityIndex < m_entities.size() && !dynamic_cast<Prey*>(m_entities[entityIndex].get()))
				++entityIndex;
			if(!m_entities[entityIndex].get()) continue;
			const auto& offset = ring_offsets[std::rand() % ring_offsets.size()];
			m_entities[entityIndex]->spawn(x + offset.x, y + offset.y);
			startEntityAgent(*m_entities[entityIndex]);
			++entityIndex;
		}
	}
}

void World::stopAgents()
{
    m_stopFlag.store(-1, std::memory_order_release);
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
		Entity* entityPtr = &agent->entity;
		m_pool.enqueue([entityPtr, &stop = m_stopFlag] {
			if (stop.load(std::memory_order_acquire) >= 0) entityPtr->tick();
		});
    }
    m_pool.wait();
	std::this_thread::sleep_for(std::chrono::microseconds(microseconds));
}

void World::startEntityAgent(Entity& entity)
{
    Point loc = entity.getLocation();
    for (auto point : entity.getDrawInfo().points)
    {
        occupiedSpace[mod((loc.x + point.x), WORLD_X)][mod((loc.y + point.y), WORLD_Y)] = &entity;
    }
    m_agents.push_back(std::make_unique<Agent>(entity));
}

bool World::validMove(Entity& entity) const
{
	Point locReq = entity.getLocationRequest();
	for(auto point : entity.getDrawInfo().points)
	{
		Entity* occupyingEntity = occupiedSpace[mod((locReq.x + point.x), WORLD_X)][mod((locReq.y + point.y), WORLD_Y)];
		if(occupyingEntity && occupyingEntity != &entity)
		{
			return entity.attack(*occupyingEntity);
		}
	}
	return true;
}

void World::updateEntityLocation(Entity& entity)
{
	Point loc = entity.getLocation();
	Point locReq = entity.getLocationRequest();
	for(auto point : entity.getDrawInfo().points)
	{
		occupiedSpace[mod((loc.x + point.x), WORLD_X)][mod((loc.y + point.y), WORLD_Y)] = nullptr;
	}
	for(auto point : entity.getDrawInfo().points)
	{
		occupiedSpace[mod((locReq.x + point.x), WORLD_X)][mod((locReq.y + point.y), WORLD_Y)] = &entity;
	}
}

void World::freeEntityLocation(Entity& entity)
{
	Point loc = entity.getLocation();
	for(auto point : entity.getDrawInfo().points)
	{
		occupiedSpace[mod((loc.x + point.x), WORLD_X)][mod((loc.y + point.y), WORLD_Y)] = nullptr;
	}
}

void World::selectBestBrains(int preyAmount, std::vector<NeuralNetworkPtr>& preyBrains, int predatorAmount, std::vector<NeuralNetworkPtr>& predatorBrains)
{
	logFile << "[INFO] Selecting best brains from " << m_deadEntities.size() << " dead entities." << std::endl;
	while (preyAmount > 0 || predatorAmount > 0)
	{
		if (m_deadEntities.empty())
		{
			logFile << "[INFO] No dead entities left to select brains from." << std::endl;
			logFile << "[INFO] Generating random neural networks for " << preyAmount << " preys and "
				<< predatorAmount << " predators." << std::endl;
			while (preyAmount-- > 0) preyBrains.push_back(std::make_unique<NeuralNetwork>());
			while (predatorAmount-- > 0) predatorBrains.push_back(std::make_unique<NeuralNetwork>());
			return;
		}
		// Top brain for next generation
		EntityPtr deadEntity = std::move(m_deadEntities.back());
		int offsping_amount = generateRandomInt(1, 3);

		// // No offspring with 5% chance and insert random brain instead


		if (preyAmount > 0 && dynamic_cast<Prey*>(deadEntity.get()))
		{
			// 60% chance that entity that didnt reproduce gets replaced with random
			if (deadEntity->getFitness() < 1 && trueWithProb(0.6))
			{
				preyBrains.push_back(std::make_unique<NeuralNetwork>());
				preyAmount--;
			}
			else
			{
				while (offsping_amount-- > 0 && preyAmount-- > 0)
				{
					preyBrains.push_back(std::make_unique<NeuralNetwork>(deadEntity->getBrain()));
				}
			}	
		}
		else if (predatorAmount > 0 && dynamic_cast<Predator*>(deadEntity.get()))
		{
			// 60% chance that entity that didnt reproduce gets replaced with random
			if (deadEntity->getFitness() < 1 && trueWithProb(0.6))
			{
				predatorBrains.push_back(std::make_unique<NeuralNetwork>());
				predatorAmount--;
			}
			else
			{
				while (offsping_amount-- > 0 && predatorAmount-- > 0)
				{
					predatorBrains.push_back(std::make_unique<NeuralNetwork>(deadEntity->getBrain()));
				}
			}
		}
		m_deadEntities.pop_back();
	}
}

Point World::getBirthLocation(Entity& parent)
{
	Point parentLocation = parent.getLocation();
	Point birthLocation = {};
	// Try to find an unoccupied space several times
	for (int i = 0; i < 20; i++)
	{
		// Choose random location around parent
		birthLocation.x = mod(parentLocation.x + (std::rand() % 41 - 20), WORLD_X);
		birthLocation.y = mod(parentLocation.y + (std::rand() % 41 - 20), WORLD_Y);

		// Check for every Point of the hitbox
		for(auto point : parent.getDrawInfo().points)
		{
			// If the space is already occupied
			Entity* occupyingEntity = occupiedSpace[mod((birthLocation.x + point.x), WORLD_X)][mod((birthLocation.y + point.y), WORLD_Y)];
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
	// Return invalid location if no valid location was found
	birthLocation.x = -1;
	birthLocation.y = -1;
	return birthLocation;
}

InputLayerValues World::generateEntityPerception(Entity& entity)
{
	InputLayerValues input_layer_values;
	Point entityLocation = entity.getLocation();
	for (auto perception_mapping : perception_mapping_matrix)
	{
		int x = perception_mapping.point.x + entityLocation.x;
		int y = perception_mapping.point.y + entityLocation.y;
		Entity* occupyingEntity = occupiedSpace[mod(x, WORLD_X)][mod(y, WORLD_Y)];
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
	for (auto& entity : m_entities)
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
	for (auto& entity : m_entities)
	{
		out_file << entity->documentSelf();
	}
	out_file.close();
}

void World::createPopulationDataFile()
{
	std::string timestamp = getCurrentTimestamp();
	std::string filename = populationDataPath + "populationData_Gen_" + std::to_string(generationCounter)
		+ "_" + timestamp + ".csv";
	populationDataFile.open(filename);

	if (!populationDataFile.is_open())
	{
        std::cerr << "Error opening population data file!" << std::endl;
		return;
    }
	populationDataFile << "Tick,Predators,Preys" << std::endl;
}
