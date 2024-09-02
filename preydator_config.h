#ifndef PREYDATOR_CONFIG_H__
#define PREYDATOR_CONFIG_H__

#include <string>
#include <map>
#include <functional>

typedef struct {
    int entity_start_health;
	int entity_reproduction_goal;
	int barriers_start_amount;
	int plants_start_amount;
	int prey_start_amount;
	int predators_start_amount;
	int tick_delay;
} PreydatorConfig;

const std::map<std::string, std::function<void(PreydatorConfig&, std::string)>>
preydator_config_assign_map
{
	{"entity_start_health", [](PreydatorConfig &config, std::string value)
		{config.entity_start_health = std::stoi(value);}},
	{"entity_reproduction_goal", [](PreydatorConfig &config, std::string value)
		{config.entity_reproduction_goal = std::stoi(value);}},
	{"barriers_start_amount", [](PreydatorConfig &config, std::string value)
		{config.barriers_start_amount = std::stoi(value);}},
	{"plants_start_amount", [](PreydatorConfig &config, std::string value)
		{config.plants_start_amount = std::stoi(value);}},
	{"prey_start_amount", [](PreydatorConfig &config, std::string value)
		{config.prey_start_amount = std::stoi(value);}},
	{"predators_start_amount", [](PreydatorConfig &config, std::string value)
		{config.predators_start_amount = std::stoi(value);}},
	{"tick_delay", [](PreydatorConfig &config, std::string value)
		{config.tick_delay = std::stoi(value);}}
};

#endif /* PREYDATOR_CONFIG_H__ */