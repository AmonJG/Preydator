#ifndef PREYDATOR_CONFIG_H__
#define PREYDATOR_CONFIG_H__

#include <string>
#include <map>
#include <functional>
#include <vector>
#include <sstream>
#include <cstdint>

#define WORLD_X 1900
#define WORLD_Y 1000

#define INVALID_INPUT_LAYER_VALUE -0.3
#define EMPTY_INPUT_LAYER_VALUE 0
#define BARRIER_INPUT_LAYER_VALUE -0.6
#define PLANT_SEEN_BY_PREY_INPUT_LAYER_VALUE 1
#define PLANT_SEEN_BY_PREDATOR_INPUT_LAYER_VALUE 0.2
#define PREY_SEEN_BY_PREY_INPUT_LAYER_VALUE 0.1
#define PREY_SEEN_BY_PREDATOR_INPUT_LAYER_VALUE 1
#define PREDATOR_SEEN_BY_PREY_INPUT_LAYER_VALUE -1
#define PREDATOR_SEEN_BY_PREDATOR_INPUT_LAYER_VALUE -0.2

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

struct PreydatorConfig
{
	bool show_animation;
    unsigned int entity_start_health;
	unsigned int entity_standard_health_loss_per_tick;
	unsigned int entity_reproduction_goal;
	unsigned int barriers_start_amount;
	unsigned int plants_start_amount;
	unsigned int prey_start_amount;
	unsigned int predators_start_amount;
	unsigned int tick_delay;
	unsigned int max_step_size;
	unsigned int max_ticks_per_generation;
	unsigned int init_hidden_layers;
	unsigned int init_neurons_per_hidden_layer;
	unsigned int init_mutations;
	unsigned int offspring_mutations;
};

extern PreydatorConfig config;

const std::map<std::string, std::function<void(std::string)>>
preydator_config_assign_map
{
	{"show_animation", [](std::string value)
		{std::istringstream(value) >> std::boolalpha >> config.show_animation;}},
	{"entity_start_health", [](std::string value)
		{config.entity_start_health = std::stoul(value);}},
	{"entity_standard_health_loss_per_tick", [](std::string value)
		{config.entity_standard_health_loss_per_tick = std::stoul(value);}},
	{"entity_reproduction_goal", [](std::string value)
		{config.entity_reproduction_goal = std::stoul(value);}},
	{"barriers_start_amount", [](std::string value)
		{config.barriers_start_amount = std::stoul(value);}},
	{"plants_start_amount", [](std::string value)
		{config.plants_start_amount = std::stoul(value);}},
	{"prey_start_amount", [](std::string value)
		{config.prey_start_amount = std::stoul(value);}},
	{"predators_start_amount", [](std::string value)
		{config.predators_start_amount = std::stoul(value);}},
	{"tick_delay", [](std::string value)
		{config.tick_delay = std::stoul(value);}},
	{"max_step_size", [](std::string value)
		{config.max_step_size = std::stoul(value);}},
	{"max_ticks_per_generation", [](std::string value)
		{config.max_ticks_per_generation = std::stoul(value);}},
	{"init_hidden_layers", [](std::string value)
		{config.init_hidden_layers = std::stoul(value);}},
	{"init_neurons_per_hidden_layer", [](std::string value)
		{config.init_neurons_per_hidden_layer = std::stoul(value);}},
	{"init_mutations", [](std::string value)
		{config.init_mutations = std::stoul(value);}},
	{"offspring_mutations", [](std::string value)
		{config.offspring_mutations = std::stoul(value);}}
};

#endif /* PREYDATOR_CONFIG_H__ */
