#ifndef PREYDATOR_CONFIG_H__
#define PREYDATOR_CONFIG_H__

#include <string>
#include <map>
#include <functional>
#include <vector>

#define WORLD_X 1900
#define WORLD_Y 1000

#define INVALID_INPUT_LAYER_VALUE 0
#define EMPTY_INPUT_LAYER_VALUE 0.2
#define BARRIER_INPUT_LAYER_VALUE 0.4
#define PLANT_INPUT_LAYER_VALUE 0.6
#define PREY_INPUT_LAYER_VALUE 0.8
#define PREDATOR_INPUT_LAYER_VALUE 1.0

struct Point
{
    int x, y;
};

struct Color
{
	uint8_t r, g, b, a;
};

struct DrawInfo
{
    std::vector<Point> points;
    Color color;
};

struct PreydatorConfig {
    int entity_start_health;
	int entity_reproduction_goal;
	int barriers_start_amount;
	int plants_start_amount;
	int prey_start_amount;
	int predators_start_amount;
	int tick_delay;
	int init_hidden_layers;
	int init_neurons_per_hidden_layer;
	int init_mutations;
	int offspring_mutations;
};

extern PreydatorConfig config;

const std::map<std::string, std::function<void(std::string)>>
preydator_config_assign_map
{
	{"entity_start_health", [](std::string value)
		{config.entity_start_health = std::stoi(value);}},
	{"entity_reproduction_goal", [](std::string value)
		{config.entity_reproduction_goal = std::stoi(value);}},
	{"barriers_start_amount", [](std::string value)
		{config.barriers_start_amount = std::stoi(value);}},
	{"plants_start_amount", [](std::string value)
		{config.plants_start_amount = std::stoi(value);}},
	{"prey_start_amount", [](std::string value)
		{config.prey_start_amount = std::stoi(value);}},
	{"predators_start_amount", [](std::string value)
		{config.predators_start_amount = std::stoi(value);}},
	{"tick_delay", [](std::string value)
		{config.tick_delay = std::stoi(value);}},
	{"init_hidden_layers", [](std::string value)
		{config.init_hidden_layers = std::stoi(value);}},
	{"init_neurons_per_hidden_layer", [](std::string value)
		{config.init_neurons_per_hidden_layer = std::stoi(value);}},
	{"init_mutations", [](std::string value)
		{config.init_mutations = std::stoi(value);}},
	{"offspring_mutations", [](std::string value)
		{config.offspring_mutations = std::stoi(value);}}
};

#endif /* PREYDATOR_CONFIG_H__ */