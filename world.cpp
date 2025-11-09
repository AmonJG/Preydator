#include "Entity/entity.h"
#include "Entity/plant.h"
#include "Entity/prey.h"
#include "Entity/predator.h"
#include "graphics_handler.h"
#include "preydator_math.h"
#include "world.h"
#include <condition_variable>
#include <mutex>
#include <thread>
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

static EntityPtr occupiedSpace[WORLD_X][WORLD_Y] = {nullptr};
static unsigned int tickCounter = 0;
static unsigned int generationCounter = 0;
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
{
	if (config.show_animation)
	{
		m_graphicsHandler = GraphicsHandler::GetInstance(WORLD_X - 1, WORLD_Y - 1);
	}

	std::string timestamp = getCurrentTimestamp();
	dataPath = "data/exec_" + timestamp;
	std::filesystem::create_directory(dataPath);
	dataPath += "/";

    std::string filename = dataPath + "populationData_" + timestamp + ".csv";
    populationDataFile.open(filename);
	filename = dataPath + "preydator_" + timestamp + ".log";
	logFile.open(filename);

    if (!populationDataFile.is_open())
	{
        std::cerr << "Error opening population data file!" << std::endl;
    }
	if (!logFile.is_open())
	{
        std::cerr << "Error opening log file!" << std::endl;
    }
	populationDataFile << "Tick,Predators,Preys,Plants" << std::endl;
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
	std::vector<NeuralNetwork> preyBrains;
	std::vector<NeuralNetwork> predatorBrains;
	selectBestBrains(preyBrains, predatorBrains);
	initGeneration(preyBrains, predatorBrains);
}

bool World::initSavedGeneration(std::string const& in_file_path)
{
	logFile << "[INFO] Execution initialized with input file: " << in_file_path << std::endl;
	std::vector<NeuralNetwork> preyBrains;
	std::vector<NeuralNetwork> predatorBrains;
	std::ifstream in_file;
	in_file.open(in_file_path);
	if (!in_file.is_open())
	{
		std::cerr << "Error opening input file: " << in_file_path << std::endl;
		return false;
	}
	bool isPreyBrain = false;
	for (std::string line; std::getline(in_file, line);)
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
		NeuralNetwork brain = NeuralNetwork(in_file);
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
			preyBrains.emplace_back();
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
			predatorBrains.emplace_back();
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
	logFile << "[INFO] Killing " << m_agents.size() << " agents" << std::endl;
	stopAgents();
	m_agents.clear();
	m_entities.clear();
	m_barriers.clear();

	for (int i = 0; i < WORLD_X; ++i) {
        for (int j = 0; j < WORLD_Y; ++j) {
            occupiedSpace[i][j] = nullptr;
        }
    }
	m_barrierPoints.clear();
	tickCounter = 0;
}

bool World::generationAlive()
{
	return m_generationAlive;
}

void World::updateAgents()
{
    std::vector<std::vector<Agent>::iterator> agentsToTerminate;
	std::vector<EntityPtr> entitiesToStart;
	int plantPopulation = 0;
	int preyPopulation = 0;
	int predatorPopulation = 0;
    for(std::vector<Agent>::iterator itr = m_agents.begin(); itr != m_agents.end(); itr++)
    {
		if(std::dynamic_pointer_cast<Plant>((*itr).entity))plantPopulation++;
		if(std::dynamic_pointer_cast<Prey>((*itr).entity))preyPopulation++;
		if(std::dynamic_pointer_cast<Predator>((*itr).entity))predatorPopulation++;
		// If entity executes a valid move
		if(validMove((*itr).entity))
		{
			// Allow location update
			(*itr).entity->allowLocationUpdate();
			updateEntityLocation((*itr).entity);
		}
		// If entity can reproduce
		if((*itr).entity->check() & 0x2)
		{
			Point birthLocation = getBirthLocation((*itr).entity);
			if (birthLocation.x >= 0 && birthLocation.y >= 0)
			{
				entitiesToStart.push_back((*itr).entity->giveBirth(birthLocation));
				logFile << "Entity " << (*itr).entity->getId() << " produced offspring entity "
					<< entitiesToStart.back()->getId() << std::endl;
			}
		}
		// If entity has no healt
		if((*itr).entity->check() & 0x1)
		{
			// kill entity
			(*itr).entity->sendSignal(1);
			//TODO: can a thread even be joinable right after the signal without a tick?
			if((*itr).thread.joinable())
			{
				agentsToTerminate.push_back(itr);
			}
			continue;
		}
		//Send perception data to brain (input layer of neural network)
		(*itr).entity->perceive(generateEntityPerception((*itr).entity));
    }
	populationDataFile << tickCounter << "," << predatorPopulation << "," << preyPopulation << "," << plantPopulation << std::endl;
	if (predatorPopulation == 0 || preyPopulation == 0) m_generationAlive = false;
	// Execute one World tick
    tick(config.tick_delay);
	if (tickCounter >= config.max_ticks_per_generation) m_generationAlive = false;
	// Kill and remove all joinable Agents and their threads from last tick
    for(auto agent = agentsToTerminate.rbegin(); agent != agentsToTerminate.rend(); ++agent)
    {
		if (!std::dynamic_pointer_cast<Plant>((**agent).entity))
		{
			m_deadEntities.push((**agent).entity);
		}
        //logFile << "join: " << (**agent).entity->getId() << std::endl;
        (**agent).thread.join();
        //logFile << "agents size: " << m_agents.size() << std::endl;
        //logFile << "erase: " << (**agent).entity->getId() << std::endl;
		m_agents.erase(*agent);
        //logFile << "agents size: " << m_agents.size() << std::endl;
    }
	// Start new agents that have been born
	for(auto entity : entitiesToStart)
	{
		std::lock_guard<std::mutex> lk(m_mtx);
		m_entities.push_back(entity);
		startEntityAgent(entity);
		logFile << "Started newborn entity: " << entity->getId() << std::endl;
	}
}

void World::drawEntities()
{
	if (!config.show_animation) return;
	m_graphicsHandler->drawPoints(m_barrierPoints, {128, 128, 128, 255});
    for(auto& agent : m_agents)
    {
        m_graphicsHandler->drawEntity(*agent.entity);
    }
    m_graphicsHandler->render();
}

void World::initGeneration(std::vector<NeuralNetwork> const& preyBrains, std::vector<NeuralNetwork> const& predatorBrains)
{
	std::string currentTimestamp = getCurrentTimestamp();
	std::cout << currentTimestamp << " Generation: " << ++generationCounter << std::endl;
	logFile << currentTimestamp << " Generation: " << generationCounter << std::endl;
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
    m_stopFlag.store(0, std::memory_order_relaxed);
    std::lock_guard<std::mutex> lk(m_mtx);
    for(auto& entity : m_entities)
    {
		entity->spawn();
		startEntityAgent(entity);
    }
    m_cv.notify_all();
	logFile << "[INFO] All " << m_agents.size()
		<< " agents have been started for current generation." << std::endl;
}

void World::stopAgents()
{
    m_stopFlag.store(-1, std::memory_order_relaxed);
	createEntitySaveFile();
	// document one entity for testing
	//m_agents.back().entity->documentSelf();
	createNeuralNetworkGraphs();
    tick(1000);
	std::cout << std::endl;
    for(auto& agent : m_agents)
    {
		logFile << "Join agent: " << agent.entity->getId() << std::endl;
        agent.thread.join();
    }
	logFile << "[INFO] All " << m_agents.size()
		<< " agents have been stopped for current generation." << std::endl;
}

void World::tick(int microseconds)
{
	std::cout << "\rTick: " << tickCounter++ << "/" << config.max_ticks_per_generation << std::flush;
	{
		//std::lock_guard<std::mutex> lk(m_mtx);
		m_haltAgents2 = true;
		m_haltAgents = false;
	}
	m_cv.notify_all();
    std::this_thread::sleep_for(std::chrono::microseconds(microseconds));
    {
		//std::lock_guard<std::mutex> lk(m_mtx);
		m_haltAgents = true;
		m_haltAgents2 = false;
	}
	m_cv.notify_all();
    std::this_thread::sleep_for(std::chrono::microseconds(microseconds));
}

void World::startEntityAgent(EntityPtr entity)
{
	if(!entity) return;
	entity->setSharedData(&m_cv, &m_mtx, &m_haltAgents, &m_haltAgents2);
	Point loc = entity->getLocation();
	for(auto point : entity->getDrawInfo().points)
	{
		occupiedSpace[loc.x + point.x][loc.y + point.y] = entity;
	}
	m_agents.push_back({entity, std::thread(&Entity::run, entity, std::ref(m_stopFlag))});
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


void World::selectBestBrains(std::vector<NeuralNetwork>& preyBrains, std::vector<NeuralNetwork>& predatorBrains)
{
	int preyAmount = config.prey_start_amount;
	int predatorAmount = config.predators_start_amount;
	while (preyAmount || predatorAmount)
	{
		if (m_deadEntities.empty())
		{
			logFile << "[INFO] Execution initialized without old entities." << std::endl;
			logFile << "[INFO] Generating random neural networks..." << std::endl;
			while (preyAmount-- > 0) preyBrains.emplace_back();
			while (predatorAmount-- > 0) predatorBrains.emplace_back();
			return;
		}
		EntityPtr deadEntity = m_deadEntities.top();
		if (preyAmount > 0 && std::dynamic_pointer_cast<Prey>(deadEntity))
		{
			preyBrains.push_back(deadEntity->getBrain());
			preyAmount--;
		}
		else if (predatorAmount > 0 && std::dynamic_pointer_cast<Predator>(deadEntity))
		{
			predatorBrains.push_back(deadEntity->getBrain());
			predatorAmount--;
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
				input_layer_values.push_back({perception_mapping.node_id, occupyingEntity->getPerceptionValue()});
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
    }
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
	}
	for (auto entity : m_entities)
	{
		out_file << entity->documentSelf();
	}
	out_file.close();
}