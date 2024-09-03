#include "neural_network.h"

NeuralNetwork::NeuralNetwork()
{
	// Create random brain
}

NeuralNetwork::NeuralNetwork(NeuralNetwork const& parent_brain)
{
	// Mutate given brain
}

NeuralNetwork::~NeuralNetwork()
{

}

void NeuralNetwork::initializeInputLayer(InputLayerValues input_layer_values)
{
	for (auto node : input_layer_values)
	{
		m_input_layer[node.id] = node.value;
	}
}

Point NeuralNetwork::decideMovement()
{
	Point desired_movement;
	desired_movement.x = (std::rand() % 5) - 2;
	desired_movement.y = (std::rand() % 5) - 2;
	return desired_movement;
}

/*
LayerValues NeuralNetwork::fire(LayerValues input_layer_values)
{
	// TODO: Assign input layer

	// Assign hidden layers
	for (auto hidden_layer : m_hidden_layers)
	{
		for (auto neuron : hidden_layer.neurons)
		{
			neuron->value = 0;
			for (auto edge : neuron.input_edges)
			{
				neuron->value += edge.weight * edge.src_neuron->value;
			}
		}
	}
	// Assign output layer
	LayerValues output_layer_values;
	for (auto neuron : m_output_layer.neurons)
	{
		neuron->value = 0;
		for (auto edge : neuron.input_edges)
		{
			neuron->value += edge.weight * edge.src_neuron->value;
			output_layer_values.push_back({neuron->id, neuron->value});
		}
	}
	return output_layer_values;
}
*/

void NeuralNetwork::mutate()
{
}