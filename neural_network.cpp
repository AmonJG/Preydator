#include "neural_network.h"
#include "preydator_math.h"
#include <iostream>
#include <fstream>
#include <cmath>

NeuralNetwork::NeuralNetwork()
{
	// Create empty input layer
	for (auto perception_mapping : perception_mapping_matrix)
	{
		Neuron neuron = std::make_shared<Node>();
		neuron->id = m_neuron_id_counter++;
		neuron->value = INVALID_INPUT_LAYER_VALUE;
		m_input_layer[perception_mapping.node_id] = neuron;
	}

	// Create two hidden layers with one neuron
	Layer hidden_layer_0;
	Neuron neuron_0 = std::make_shared<Node>();
	neuron_0->id = m_neuron_id_counter++;
	neuron_0->value = 0;
	hidden_layer_0.neurons.push_back(neuron_0);
	Layer hidden_layer_1;
	Neuron neuron_1 = std::make_shared<Node>();
	neuron_1->id = m_neuron_id_counter++;
	neuron_1->value = 0;
	hidden_layer_1.neurons.push_back(neuron_1);
	m_hidden_layers.push_back(hidden_layer_0);
	m_hidden_layers.push_back(hidden_layer_1);

	// Create empty output layer
	// TODO put all in two lines
	Neuron neuron_x = std::make_shared<Node>();
	neuron_x->id = m_neuron_id_counter++;
	neuron_x->value = INVALID_INPUT_LAYER_VALUE;
	m_output_layer[OutputLayerNodeIds::MOVE_X] = neuron_x;
	Neuron neuron_y = std::make_shared<Node>();
	neuron_y->id = m_neuron_id_counter++;
	neuron_y->value = INVALID_INPUT_LAYER_VALUE;
	m_output_layer[OutputLayerNodeIds::MOVE_Y] = neuron_y;

	// Create random brain
	init();
}

NeuralNetwork::NeuralNetwork(NeuralNetwork const& parent_brain)
{
	m_neuron_id_counter = parent_brain.getNeuronIdCounter();
	// Mutate given brain
}

NeuralNetwork::~NeuralNetwork()
{

}

Point NeuralNetwork::decideMovement(InputLayerValues input_layer_values)
{
	Point desired_movement;
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
	calculateNeuronValue(x_out);
	Neuron y_out = m_output_layer[OutputLayerNodeIds::MOVE_Y];
	calculateNeuronValue(y_out);

	// Normalize and assign output values (range = [-9,9])
	
	// TODO
	// TODO: REMOVE RANDOM VALUE OVERWRITE
	// TODO
	
	desired_movement.x = std::tanh(x_out->value) * 9;
	//desired_movement.x = (std::rand() % 5) - 2;
	desired_movement.y = std::tanh(y_out->value) * 9;
	//desired_movement.y = (std::rand() % 5) - 2;
	return desired_movement;
}

int NeuralNetwork::getNeuronIdCounter() const
{
	return m_neuron_id_counter;
}

void NeuralNetwork::exportGraph()
{
	std::ofstream out_file;
	std::string timestamp = getCurrentTimestamp();
	std::string filename = "data/neuralNetworkGraph_" + timestamp + ".dot";
	out_file.open(filename);

    if (!out_file.is_open())
	{
        std::cerr << "Error opening neural network graph export file!" << std::endl;
    }
	out_file << "graph {\n\trankdir=LR;\n\tsubgraph {\n\t\trank=same;" << std::endl;
	// write input layer neurons
	for (auto perception_mapping : perception_mapping_matrix)
	{
		Neuron input_neuron = m_input_layer[perception_mapping.node_id];
		out_file << "\t\t" << input_neuron->id << " [label = \"" <<
			perception_mapping.id_name << "\"];" << std::endl;
	}
	out_file << "\t}\n\tsubgraph {\n\t\trank=same;" << std::endl;
	// Write output layer neurons
	// TODO: remove hardcoded labels
	Neuron output_neuron = m_output_layer[OutputLayerNodeIds::MOVE_X];
	out_file << "\t\t" << output_neuron->id << " [label = \"MOVE_X\"];" << std::endl;
	output_neuron = m_output_layer[OutputLayerNodeIds::MOVE_Y];
	out_file << "\t\t" << output_neuron->id << " [label = \"MOVE_Y\"];" << std::endl;
	// Write hidden layer neurons
	for (size_t i = 0; i < m_hidden_layers.size(); i++)
	{
		out_file << "\t}\n\tsubgraph {\n\t\trank=same;" << std::endl;
		for (auto neuron : m_hidden_layers[i].neurons)
		{
			out_file << "\t\t" << neuron->id << " [label = \"HL " <<
			i << ": " << neuron->id << "\"];" << std::endl;
		}
	}
	out_file << "\t}" << std::endl;
	// Write synapses
	for (auto synapse : m_synapses)
	{
		out_file << "\t" << synapse->src_neuron->id << "--" <<
			synapse->dst_neuron->id << " [label = \"" <<
			synapse->weight << "\"];" << std::endl;
	}
	out_file << "}" << std::endl;
	out_file.close();
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
	neuron->value = 0;
	for (auto edge : neuron->incoming_edges)
	{
		neuron->value += edge->weight * edge->src_neuron->value;
	}
}

Neuron NeuralNetwork::getRandNeuronFromFollowingLayers(size_t startLayerIndex)
{
	Neuron neuron;
	size_t dstLayerIndex = generateRandomInt(startLayerIndex, m_hidden_layers.size());
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

void NeuralNetwork::init()
{
	for (auto perception_mapping : perception_mapping_matrix)
	{
		Synapse synapse = std::make_shared<Edge>();
		m_synapses.push_back(synapse);
		synapse->weight = generateRandomDouble(-1, 1);
		synapse->src_neuron = m_input_layer[perception_mapping.node_id];
		synapse->dst_neuron = getRandNeuronFromFollowingLayers(0);
		// TODO: check ob das wirklich so rekursiv geht
		synapse->dst_neuron->incoming_edges.push_back(synapse);
	}
	for (size_t i = 0; i < m_hidden_layers.size(); i++)
	{
		for (auto neuron : m_hidden_layers[i].neurons)
		{
			Synapse synapse = std::make_shared<Edge>();
			m_synapses.push_back(synapse);
			synapse->weight = generateRandomDouble(-1, 1);
			synapse->src_neuron = neuron;
			synapse->dst_neuron = getRandNeuronFromFollowingLayers(i+1);
			// TODO: check ob das wirklich so rekursiv geht
			synapse->dst_neuron->incoming_edges.push_back(synapse);
		}
	}
	//for (int i = 0; i < config.init_mutations; i++) mutate();
}

void NeuralNetwork::mutate()
{
	int numberOfEdgesToMutate = randWithExponentialBias(m_synapses.size());
	auto edgesToMutate = generateUniqueRandInts(numberOfEdgesToMutate, m_synapses.size());
	for (auto edgeIndex : edgesToMutate)
	{
		// max change in weight: +/-2.5%
		double changeFactor = m_synapses[edgeIndex]->weight / 40;
		m_synapses[edgeIndex]->weight += generateRandomDouble(-changeFactor, changeFactor);
	}
	int r = std::rand() % 100;
	// 10% New Neuron
	if (r < 10)
	{
		int layerIndex = std::rand() % m_hidden_layers.size();
		Neuron neuron = std::make_shared<Node>();
		neuron->id = m_neuron_id_counter++;
		neuron->value = INVALID_INPUT_LAYER_VALUE;
		m_hidden_layers[layerIndex].neurons.push_back(neuron);
	}
	// 10% New Synapse
	if (r >= 90)
	{
		Synapse synapse = std::make_shared<Edge>();
		m_synapses.push_back(synapse);
		synapse->weight = generateRandomDouble(-1, 1);
		// +1 because input layer is also possible
		int srcLayerIndex = std::rand() % (m_hidden_layers.size() + 1);
		// src is input layer
		if (srcLayerIndex == 0)
		{
			int neuronIndex = std::rand() % perception_mapping_matrix.size();
			synapse->src_neuron = m_input_layer[perception_mapping_matrix[neuronIndex].node_id];
		}
		else
		{
			int neuronIndex = std::rand() % m_hidden_layers[srcLayerIndex - 1].neurons.size();
			synapse->src_neuron = m_hidden_layers[srcLayerIndex - 1].neurons[neuronIndex];
		}
		synapse->dst_neuron = getRandNeuronFromFollowingLayers(srcLayerIndex);
		// TODO: check ob das wirklich so rekursiv geht
		synapse->dst_neuron->incoming_edges.push_back(synapse);
	}
	// 1%   -> in new Hidden Layer
	// 2%  Delete Neuron
	// 2%  Delete Synapse
}
