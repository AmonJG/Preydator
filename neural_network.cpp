#include "neural_network.h"
#include "preydator_math.h"
#include <iostream>
#include <fstream>
#include <cmath>

NeuralNetwork::NeuralNetwork()
{
	createInputAndOutputLayer();

	// Create hidden layers with neurons
	for (unsigned int layerIndex = 0; layerIndex < config.init_hidden_layers; layerIndex++)
	{
		Layer hidden_layer;
		for (unsigned int neuronIndex = 0; neuronIndex < config.init_neurons_per_hidden_layer; neuronIndex++)
		{
			Neuron neuron = std::make_shared<Node>();
			m_neurons.push_back(neuron);
			neuron->id = m_neuron_id_counter++;
			neuron->value = 0;
			neuron->bias = generateRandomDouble(-0.2, 0.2);
			hidden_layer.neurons.push_back(neuron);
		}
		m_hidden_layers.push_back(hidden_layer);
	}

	// Create new fully connected brain with random weights
	initNewBrain();
	//std::cout << "CONSTRUCT NN ";
	// Mutate new brain as often as configured
	for (unsigned int i = 0; i < config.init_mutations; i++) mutate();
}

NeuralNetwork::NeuralNetwork(std::ifstream& in_file)
{
	createInputAndOutputLayer();
	std::vector<int> hidden_layer_sizes;
	for (std::string line; std::getline(in_file, line);)
	{
		if (line.empty()) break;
		if (line[0] == 'L')
		{
			size_t firstColon = line.find(':');
			size_t secondColon = line.find(':', firstColon + 1);
			if (firstColon == std::string::npos || secondColon == std::string::npos)
			{
				std::cerr << "ERROR: Missing ':' in node input line" << std::endl;
				exit(EXIT_FAILURE);
			}
			size_t hidden_layer_index = std::stoi(line.substr(1, firstColon - 1));
			int neuron_id = std::stoi(line.substr(firstColon + 1, secondColon - firstColon - 1));
			double bias = std::stod(line.substr(secondColon + 1));

			while (hidden_layer_index >= m_hidden_layers.size())
			{
				m_hidden_layers.push_back(Layer());
			}

			Neuron neuron = std::make_shared<Node>();
			m_neurons.push_back(neuron);
			neuron->id = neuron_id;
			neuron->value = 0;
			neuron->bias = bias;
			m_neuron_id_counter++;
			m_hidden_layers[hidden_layer_index].neurons.push_back(neuron);
		}
		else if (line[0] == 'E')
		{
			size_t firstColon = line.find(':');
			size_t secondColon = line.find(':', firstColon + 1);
			if (firstColon == std::string::npos || secondColon == std::string::npos)
			{
				std::cerr << "ERROR: Missing ':' in edge input line" << std::endl;
				exit(EXIT_FAILURE);
			}
			int src_neuron_id = std::stoi(line.substr(1, firstColon - 1));
			int dst_neuron_id = std::stoi(line.substr(firstColon + 1, secondColon - firstColon - 1));
			double weight = std::stod(line.substr(secondColon + 1));

			Synapse synapse = std::make_shared<Edge>();
			m_synapses.push_back(synapse);
			synapse->weight = weight;
			// Problem: setzt vorraus dass vector position == id
			synapse->src_neuron = m_neurons[src_neuron_id];
			synapse->dst_neuron = m_neurons[dst_neuron_id];
			// TODO: check ob das wirklich so rekursiv geht
			synapse->dst_neuron->incoming_edges.push_back(synapse);
		}
		else
		{
			std::cerr << "ERROR: Invalid input file format!" << std::endl;
			exit(EXIT_FAILURE);
		}
	}
}

NeuralNetwork::NeuralNetwork(const NeuralNetwork& other)
	: m_neuron_id_counter(other.m_neuron_id_counter)
{
	// Copy neurons and save ordering
	std::unordered_map<Neuron, Neuron> neuronMap;
	for (const auto& neuron : other.m_neurons)
	{
		auto newNeuron = std::make_shared<Node>();
		newNeuron->id = neuron->id;
		newNeuron->bias = neuron->bias;
		newNeuron->value = 0;
		neuronMap[neuron] = newNeuron;
		m_neurons.push_back(newNeuron);
	}

	// Copy input layer
	for (auto input_mapping : input_mapping_matrix)
	{
		m_input_layer[input_mapping.node_id] = neuronMap.at(other.m_input_layer.at(input_mapping.node_id));
	}
	for (auto perception_mapping : perception_mapping_matrix)
	{
		m_input_layer[perception_mapping.node_id] = neuronMap.at(other.m_input_layer.at(perception_mapping.node_id));
	}
	// Copy hidden layers
	for (const auto& layer : other.m_hidden_layers) {
		Layer newLayer;
		for (const auto& oldNeuron : layer.neurons) {
			newLayer.neurons.push_back(neuronMap.at(oldNeuron));
		}
		m_hidden_layers.push_back(newLayer);
	}
	// Copy output layer
	m_output_layer[OutputLayerNodeIds::MOVE_X] = neuronMap.at(other.m_output_layer.at(OutputLayerNodeIds::MOVE_X));
	m_output_layer[OutputLayerNodeIds::MOVE_Y] = neuronMap.at(other.m_output_layer.at(OutputLayerNodeIds::MOVE_Y));

	// Copy synapses
	for (const auto& syn : other.m_synapses)
	{
		Neuron newSrc = neuronMap.at(syn->src_neuron);
		Neuron newDst = neuronMap.at(syn->dst_neuron);
		auto newSyn = std::make_shared<Edge>();
		newSyn->weight = syn->weight;
		newSyn->src_neuron = newSrc;
		newSyn->dst_neuron = newDst;
		newDst->incoming_edges.push_back(newSyn);
		m_synapses.push_back(newSyn);
	}

	// Mutate offsping brain as often as configured
	//std::cout << "COPY BRAIN" << std::endl;
	for (unsigned int i = 0; i < config.offspring_mutations; i++) mutate();
}

NeuralNetwork::~NeuralNetwork()
{

}

Point NeuralNetwork::decideMovement(InputLayerValues perception_values, EntityInputStats entity_input_stats)
{
	InputLayerValues input_layer_values = perception_values;
	input_layer_values.push_back({InputLayerNodeIds::NOISE, gaussianNoise(0.0, 0.02)});
	double health_input = static_cast<double>(entity_input_stats.health) / config.entity_start_health;
	input_layer_values.push_back({InputLayerNodeIds::HEALTH, health_input});
	double last_step_x = StepDistanceToInputValue(entity_input_stats.location.x - entity_input_stats.previous_location.x);
	input_layer_values.push_back({InputLayerNodeIds::LAST_STEP_X, last_step_x});
	double last_step_y = StepDistanceToInputValue(entity_input_stats.location.y - entity_input_stats.previous_location.y);
	input_layer_values.push_back({InputLayerNodeIds::LAST_STEP_Y, last_step_y});
	// Set input layer according to agents perception
	setInputLayer(input_layer_values);

	// Propagate through hidden layers to calculate values
	for (auto hidden_layer : m_hidden_layers)
	{
		for (auto neuron : hidden_layer.neurons)
		{
			calculateNeuronValue(neuron);
		}
	}

	// Calculate output layer
	Neuron x_out = m_output_layer[OutputLayerNodeIds::MOVE_X];
	calculateOutputNeuronValue(x_out);
	Neuron y_out = m_output_layer[OutputLayerNodeIds::MOVE_Y];
	calculateOutputNeuronValue(y_out);

	// Normalize and assign output values (range = [ -max_step_size, max_step_size ])
	Point desired_movement;
	desired_movement.x = outputValueToStepSize(x_out->value);
	desired_movement.y = outputValueToStepSize(y_out->value);

	return desired_movement;
}

int NeuralNetwork::getNeuronIdCounter() const
{
	return m_neuron_id_counter;
}

std::string NeuralNetwork::exportGraph(std::string entity_type)
{
	std::stringstream exportedGraph;
	exportedGraph << "___" << entity_type << "___"  << std::endl;
	exportedGraph << "digraph {\n\trankdir=LR;\n\tranksep=5.0;\n\tsubgraph {\n\t\trank=same;" << std::endl;
	// write input layer neurons
	for (auto input_mapping : input_mapping_matrix)
	{
		Neuron input_neuron = m_input_layer[input_mapping.node_id];
		exportedGraph << "\t\t" << input_neuron->id << " [label = \"" <<
			input_mapping.id_name << "\"];" << std::endl;
	}
	for (auto perception_mapping : perception_mapping_matrix)
	{
		Neuron input_neuron = m_input_layer[perception_mapping.node_id];
		exportedGraph << "\t\t" << input_neuron->id << " [label = \"" <<
			perception_mapping.id_name << "\"];" << std::endl;
	}
	exportedGraph << "\t}\n\tsubgraph {\n\t\trank=same;" << std::endl;
	// Write output layer neurons
	// TODO: remove hardcoded labels
	Neuron output_neuron = m_output_layer[OutputLayerNodeIds::MOVE_X];
	exportedGraph << "\t\t" << output_neuron->id << " [label = \"MOVE_X\"];" << std::endl;
	output_neuron = m_output_layer[OutputLayerNodeIds::MOVE_Y];
	exportedGraph << "\t\t" << output_neuron->id << " [label = \"MOVE_Y\"];" << std::endl;
	// Write hidden layer neurons
	for (size_t i = 0; i < m_hidden_layers.size(); i++)
	{
		exportedGraph << "\t}\n\tsubgraph {\n\t\trank=same;" << std::endl;
		for (auto neuron : m_hidden_layers[i].neurons)
		{
			exportedGraph << "\t\t" << neuron->id << " [label = \"HL " <<
			i << ": " << neuron->id << "\"];" << std::endl;
		}
	}
	exportedGraph << "\t}" << std::endl;
	// Write synapses
	for (auto synapse : m_synapses)
	{
		exportedGraph << "\t" << synapse->src_neuron->id << "->" <<
			synapse->dst_neuron->id << " [label = \"" <<
			synapse->weight << "\"];" << std::endl;
	}
	exportedGraph << "}" << std::endl;
	return exportedGraph.str();
}

std::string NeuralNetwork::getSaveString(std::string entity_type)
{
	std::stringstream saveString;
	saveString << "___" << entity_type << "___"  << std::endl;
	for (size_t i = 0; i < m_hidden_layers.size(); i++)
	{
		for (auto neuron : m_hidden_layers[i].neurons)
		{
			saveString << "L" << i << ":" << neuron->id << ":" << neuron->bias << std::endl;
		}
	}
	for (auto synapse : m_synapses)
	{
		saveString << "E" << synapse->src_neuron->id << ":"
			<< synapse->dst_neuron->id << ":" << synapse->weight << std::endl;
	}
	saveString << std::endl;
	return saveString.str();
}

void NeuralNetwork::createInputAndOutputLayer()
{
	// Create input layer neurons
	for (auto input_mapping : input_mapping_matrix)
	{
		Neuron neuron = std::make_shared<Node>();
		m_neurons.push_back(neuron);
		neuron->id = m_neuron_id_counter++;
		neuron->value = INVALID_INPUT_LAYER_VALUE;
		neuron->bias = 0;
		m_input_layer[input_mapping.node_id] = neuron;
	}
	// Create input layer perception neurons
	for (auto perception_mapping : perception_mapping_matrix)
	{
		Neuron neuron = std::make_shared<Node>();
		m_neurons.push_back(neuron);
		neuron->id = m_neuron_id_counter++;
		neuron->value = INVALID_INPUT_LAYER_VALUE;
		neuron->bias = 0;
		m_input_layer[perception_mapping.node_id] = neuron;
	}


	// Create output layer neurons
	// TODO put all in two lines
	Neuron neuron_x = std::make_shared<Node>();
	m_neurons.push_back(neuron_x);
	neuron_x->id = m_neuron_id_counter++;
	neuron_x->value = INVALID_INPUT_LAYER_VALUE;
	neuron_x->bias = 0;
	m_output_layer[OutputLayerNodeIds::MOVE_X] = neuron_x;
	Neuron neuron_y = std::make_shared<Node>();
	m_neurons.push_back(neuron_y);
	neuron_y->id = m_neuron_id_counter++;
	neuron_y->value = INVALID_INPUT_LAYER_VALUE;
	neuron_y->bias = 0;
	m_output_layer[OutputLayerNodeIds::MOVE_Y] = neuron_y;
}

void NeuralNetwork::setInputLayer(InputLayerValues input_layer_values)
{
	for (auto input_node : input_layer_values)
	{
		if(!m_input_layer[input_node.id])
		{
			std::cerr << "Input layer nullptr!" << std::endl;
			continue;
		}
		m_input_layer[input_node.id]->value = input_node.value;
	}
}

void NeuralNetwork::calculateNeuronValue(Neuron neuron)
{
	double sum = 0;
	for (auto& weakEdge : neuron->incoming_edges)
	{
		if (auto edge = weakEdge.lock())
		{
			sum += edge->weight * edge->src_neuron->value;
		}
	}
	sum += neuron->bias;
	neuron->value = ReLU(sum);
}

void NeuralNetwork::calculateOutputNeuronValue(Neuron neuron)
{
	neuron->value = 0;
	for (auto& weakEdge : neuron->incoming_edges)
	{

		if (auto edge = weakEdge.lock())
		{
			neuron->value += edge->weight * edge->src_neuron->value;
		}
	}
}
/*
Neuron NeuralNetwork::getRandInputLayerNeuron()
{
	int neuronIndex = std::rand() % perception_mapping_matrix.size();
	return m_input_layer[perception_mapping_matrix[neuronIndex].node_id];
}
*/

Neuron NeuralNetwork::getRandHiddenLayerNeuron(size_t hiddenLayerIndex)
{
	int neuronIndex = std::rand() % m_hidden_layers[hiddenLayerIndex].neurons.size();
	return m_hidden_layers[hiddenLayerIndex].neurons[neuronIndex];
}

Neuron NeuralNetwork::getRandNeuronFromFollowingLayers(size_t startLayerIndex)
{
	Neuron neuron;
	// nearer layers are exp. more likely
	size_t dstLayerIndex = randWithExponentialBias(startLayerIndex, m_hidden_layers.size());
	// Neuron from output layer
	if (dstLayerIndex == m_hidden_layers.size())
	{
		int neuronIndex = std::rand() % m_output_layer.size();
		if (neuronIndex == 0)
		{
			neuron = m_output_layer[OutputLayerNodeIds::MOVE_X];
		}
		if (neuronIndex == 1)
		{
			neuron = m_output_layer[OutputLayerNodeIds::MOVE_Y];
		}
	}
	// Neuron from a hidden layer
	else
	{
		int neuronIndex = std::rand() % m_hidden_layers[dstLayerIndex].neurons.size();
		neuron = m_hidden_layers[dstLayerIndex].neurons[neuronIndex];
	}
	return neuron;
}

void NeuralNetwork::initNewBrain()
{
	// Connect input layer to first hidden layer
	for (auto neuron : m_hidden_layers[0].neurons)
	{
		for (auto input_mapping : input_mapping_matrix)
		{
			Synapse synapse = std::make_shared<Edge>();
			m_synapses.push_back(synapse);
			synapse->weight = generateRandomDouble(-1, 1);
			synapse->src_neuron = m_input_layer[input_mapping.node_id];
			synapse->dst_neuron = neuron;
			synapse->dst_neuron->incoming_edges.push_back(synapse);
		}

		for (auto perception_mapping : perception_mapping_matrix)
		{
			Synapse synapse = std::make_shared<Edge>();
			m_synapses.push_back(synapse);
			synapse->weight = generateRandomDouble(-1, 1);
			synapse->src_neuron = m_input_layer[perception_mapping.node_id];
			synapse->dst_neuron = neuron;
			synapse->dst_neuron->incoming_edges.push_back(synapse);
		}
	}

	// Connect hidden layers to following hidden layers
	for (size_t i = 1; i < m_hidden_layers.size(); i++)
	{
		for (auto next_neuron : m_hidden_layers[i].neurons)
		{
			for (auto prev_neuron : m_hidden_layers[i-1].neurons)
			{
				Synapse synapse = std::make_shared<Edge>();
				m_synapses.push_back(synapse);
				synapse->weight = generateRandomDouble(-1, 1);
				synapse->src_neuron = prev_neuron;
				synapse->dst_neuron = next_neuron;
				synapse->dst_neuron->incoming_edges.push_back(synapse);
			}
		}
	}

	// Connect last hidden layer to ouptut layer
	for (auto neuron : m_hidden_layers[m_hidden_layers.size() - 1].neurons)
	{
		Synapse synapse = std::make_shared<Edge>();
		m_synapses.push_back(synapse);
		synapse->weight = generateRandomDouble(-1, 1);
		synapse->src_neuron = neuron;
		synapse->dst_neuron = m_output_layer[OutputLayerNodeIds::MOVE_X];
		synapse->dst_neuron->incoming_edges.push_back(synapse);
		synapse = std::make_shared<Edge>();
		m_synapses.push_back(synapse);
		synapse->weight = generateRandomDouble(-1, 1);
		synapse->src_neuron = neuron;
		synapse->dst_neuron = m_output_layer[OutputLayerNodeIds::MOVE_Y];
		synapse->dst_neuron->incoming_edges.push_back(synapse);
	}
}

void NeuralNetwork::mutate()
{
	for (auto synapse : m_synapses)
	{
		// 15% chance for mutation
		if (trueWithProb(0.15))
		{
			double sigma = 0.1;
			// 10% chance for strong mutation
			if (trueWithProb(0.1)) sigma = 0.5;
			synapse->weight += gaussianNoise(0, sigma);
			synapse->weight = std::clamp(synapse->weight, -5.0, 5.0);
		}
	}
	for (auto neuron : m_neurons)
	{
		// 15% chance for mutation
		if (trueWithProb(0.15))
		{
			double sigma = 0.1;
			// 10% chance for strong mutation
			if (trueWithProb(0.1)) sigma = 0.5;
			neuron->bias += gaussianNoise(0, sigma);
			neuron->bias = std::clamp(neuron->bias, -5.0, 5.0);
		}
	}
/*


PLAN:

Hidden layers: 1
Hidden neurons: 16–32
Weight mutation per connection: 10–20% chance, small change ±0.1–0.2
Add edge: 5–10% per genome
Remove edge: 1–5% per genome
Add node: 1–3% per genome
Remove node: 0.5–2% per genome
Skip connections: 5–10% chance


	// 10% New Neuron with two synapses
	if (r % 10 == 0)
	{
		int hiddenLayerIndex = std::rand() % m_hidden_layers.size();

		Synapse in = std::make_shared<Edge>();
		Synapse out = std::make_shared<Edge>();
		m_synapses.push_back(in);
		m_synapses.push_back(out);
		in->weight = generateRandomDouble(-1, 1);
		out->weight = generateRandomDouble(-1, 1);

		Neuron neuron = std::make_shared<Node>();
		m_neurons.push_back(neuron);
		neuron->id = m_neuron_id_counter++;
		neuron->value = INVALID_INPUT_LAYER_VALUE;
		neuron->incoming_edges.push_back(in);
		m_hidden_layers[hiddenLayerIndex].neurons.push_back(neuron);
		in->dst_neuron = neuron;
		out->src_neuron = neuron;

		// Get random src neuron
		if (hiddenLayerIndex == 0)
		{
			in->src_neuron = getRandInputLayerNeuron();
		}
		else
		{
			in->src_neuron = getRandHiddenLayerNeuron(hiddenLayerIndex - 1);
		}
		// Get random dst neuron
		out->dst_neuron = getRandNeuronFromFollowingLayers(hiddenLayerIndex + 1);
		// TODO: check ob das wirklich so rekursiv geht
		out->dst_neuron->incoming_edges.push_back(out);
	}
	// 10% New Synapse
	if (r % 10 == 1)
	{
		Synapse synapse = std::make_shared<Edge>();
		m_synapses.push_back(synapse);
		synapse->weight = generateRandomDouble(-1, 1);
		// +1 because input layer is also possible
		int srcLayerIndex = std::rand() % (m_hidden_layers.size() + 1);
		// src is input layer
		if (srcLayerIndex == 0)
		{
			synapse->src_neuron = getRandInputLayerNeuron();
		}
		else
		{
			synapse->src_neuron = getRandHiddenLayerNeuron(srcLayerIndex - 1);
		}
		synapse->dst_neuron = getRandNeuronFromFollowingLayers(srcLayerIndex);
		// TODO: check ob das wirklich so rekursiv geht
		synapse->dst_neuron->incoming_edges.push_back(synapse);
	}
	// 10% Delete Neuron
	if (r % 10 == 2)
	{
		int layerIndex = std::rand() % (m_hidden_layers.size());
		auto neurons = m_hidden_layers[layerIndex].neurons;
		int neuronIndex = std::rand() % neurons.size();
		Neuron neuronToDelete = neurons[neuronIndex];

		for (size_t i = 0; i < m_synapses.size(); i++)
		{
			if (m_synapses[i]->src_neuron == neuronToDelete ||
				m_synapses[i]->dst_neuron == neuronToDelete)
			{
				m_synapses.erase(m_synapses.begin() + i);
			}
		}
		for (size_t i = 0; i < m_neurons.size(); i++)
		{
			if (m_neurons[i] == neuronToDelete)
			{
				m_neurons.erase(m_neurons.begin() + i);
			}
		}
		neurons.erase(neurons.begin() + neuronIndex);
	}
	// 10% Delete Synapse
	if (r % 10 == 3)
	{
		int synapseIndex = std::rand() % (m_synapses.size());
		Synapse synapseToDelete = m_synapses[synapseIndex];
		// The Synapse that is about to be removed points to a neuron.
		// This neuron has a list of all incoming edges.
		// This Synapse need to be removed from this list to be deleted.
		auto dstNeuronIncEdges = synapseToDelete->dst_neuron->incoming_edges;
		for (size_t i = 0; i < dstNeuronIncEdges.size(); i++)
		{
			if (dstNeuronIncEdges[i] == synapseToDelete)
			{
				dstNeuronIncEdges.erase(dstNeuronIncEdges.begin() + i);
			}
		}
		m_synapses.erase(m_synapses.begin() + synapseIndex);
	}
*/
	// 1%   -> in new Hidden Layer
}
