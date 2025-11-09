#ifndef PREYDATOR_NEURAL_NETWORK_H__
#define PREYDATOR_NEURAL_NETWORK_H__

#include "preydator_config.h"
#include <memory>

enum InputLayerNodeIds
{
	                M08M15, M03M17, P02M17, P06M17, P11M17, P16M15,
	        M13M11, M08M11, M03M11, P02M11, P06M11, P11M11, P16M11, P21M11,
	M16M05, M13M05, M08M05, M03M05, P02M05, P06M05, P11M05, P16M05, P21M05, P24M05,
	M18P01, M13P01, M08P01, M03P01,                 P11P01, P16P01, P21P01, P26P01,
	M18P07, M13P07, M08P07, M03P07,                 P11P07, P16P07, P21P07, P26P07,
	M16P13, M13P13, M08P13, M03P13, P02P13, P06P13, P11P13, P16P13, P21P13, P24P13,
	        M13P19, M08P19, M03P19, P02P19, P06P19, P11P19, P16P19, P21P19, 
	                M08P23, M03P25, P02P25, P06P25, P11P25, P16P23
};

struct PerceptionMapping {
    Point point;
    InputLayerNodeIds node_id;
	std::string id_name;
};

const std::vector<PerceptionMapping> perception_mapping_matrix
{
	{{-8, -15}, InputLayerNodeIds::M08M15, "M08M15"},
    {{-3, -17}, InputLayerNodeIds::M03M17, "M03M17"},
    {{ 2, -17}, InputLayerNodeIds::P02M17, "P02M17"},
    {{ 6, -17}, InputLayerNodeIds::P06M17, "P06M17"},
    {{11, -17}, InputLayerNodeIds::P11M17, "P11M17"},
    {{16, -15}, InputLayerNodeIds::P16M15, "P16M15"},

    {{-13,-11}, InputLayerNodeIds::M13M11, "M13M11"},
    {{-8, -11}, InputLayerNodeIds::M08M11, "M08M11"},
    {{-3, -11}, InputLayerNodeIds::M03M11, "M03M11"},
    {{ 2, -11}, InputLayerNodeIds::P02M11, "P02M11"},
    {{ 6, -11}, InputLayerNodeIds::P06M11, "P06M11"},
    {{11, -11}, InputLayerNodeIds::P11M11, "P11M11"},
    {{16, -11}, InputLayerNodeIds::P16M11, "P16M11"},
    {{21, -11}, InputLayerNodeIds::P21M11, "P21M11"},

    {{-16, -5}, InputLayerNodeIds::M16M05, "M16M05"},
    {{-13, -5}, InputLayerNodeIds::M13M05, "M13M05"},
    {{-8,  -5}, InputLayerNodeIds::M08M05, "M08M05"},
    {{-3,  -5}, InputLayerNodeIds::M03M05, "M03M05"},	
    {{ 2,  -5}, InputLayerNodeIds::P02M05, "P02M05"},
    {{ 6,  -5}, InputLayerNodeIds::P06M05, "P06M05"},
    {{11,  -5}, InputLayerNodeIds::P11M05, "P11M05"},
    {{16,  -5}, InputLayerNodeIds::P16M05, "P16M05"},
    {{21,  -5}, InputLayerNodeIds::P21M05, "P21M05"},
    {{24,  -5}, InputLayerNodeIds::P24M05, "P24M05"},

    {{-18,  1}, InputLayerNodeIds::M18P01, "M18P01"},
    {{-13,  1}, InputLayerNodeIds::M13P01, "M13P01"},
    {{-8,   1}, InputLayerNodeIds::M08P01, "M08P01"},
    {{-3,   1}, InputLayerNodeIds::M03P01, "M03P01"},
    {{11,   1}, InputLayerNodeIds::P11P01, "P11P01"},
    {{16,   1}, InputLayerNodeIds::P16P01, "P16P01"},
    {{21,   1}, InputLayerNodeIds::P21P01, "P21P01"},
    {{26,   1}, InputLayerNodeIds::P26P01, "P26P01"},

    {{-18,  7}, InputLayerNodeIds::M18P07, "M18P07"},
    {{-13,  7}, InputLayerNodeIds::M13P07, "M13P07"},
    {{-8,   7}, InputLayerNodeIds::M08P07, "M08P07"},
    {{-3,   7}, InputLayerNodeIds::M03P07, "M03P07"},
    {{11,   7}, InputLayerNodeIds::P11P07, "P11P07"},
    {{16,   7}, InputLayerNodeIds::P16P07, "P16P07"},
    {{21,   7}, InputLayerNodeIds::P21P07, "P21P07"},
    {{26,   7}, InputLayerNodeIds::P26P07, "P26P07"},

    {{-16, 13}, InputLayerNodeIds::M16P13, "M16P13"},
    {{-13, 13}, InputLayerNodeIds::M13P13, "M13P13"},
    {{-8,  13}, InputLayerNodeIds::M08P13, "M08P13"},
    {{-3,  13}, InputLayerNodeIds::M03P13, "M03P13"},
    {{ 2,  13}, InputLayerNodeIds::P02P13, "P02P13"},
    {{ 6,  13}, InputLayerNodeIds::P06P13, "P06P13"},
    {{11,  13}, InputLayerNodeIds::P11P13, "P11P13"},
    {{16,  13}, InputLayerNodeIds::P16P13, "P16P13"},
    {{21,  13}, InputLayerNodeIds::P21P13, "P21P13"},
    {{24,  13}, InputLayerNodeIds::P24P13, "P24P13"},

    {{-13, 19}, InputLayerNodeIds::M13P19, "M13P19"},
    {{-8,  19}, InputLayerNodeIds::M08P19, "M08P19"},
    {{-3,  19}, InputLayerNodeIds::M03P19, "M03P19"},
    {{ 2,  19}, InputLayerNodeIds::P02P19, "P02P19"},
    {{ 6,  19}, InputLayerNodeIds::P06P19, "P06P19"},
    {{11,  19}, InputLayerNodeIds::P11P19, "P11P19"},
    {{16,  19}, InputLayerNodeIds::P16P19, "P16P19"},
    {{21,  19}, InputLayerNodeIds::P21P19, "P21P19"},

    {{-8,  23}, InputLayerNodeIds::M08P23, "M08P23"},
    {{-3,  25}, InputLayerNodeIds::M03P25, "M03P25"},
    {{ 2,  25}, InputLayerNodeIds::P02P25, "P02P25"},
    {{ 6,  25}, InputLayerNodeIds::P06P25, "P06P25"},
    {{11,  25}, InputLayerNodeIds::P11P25, "P11P25"},
    {{16,  23}, InputLayerNodeIds::P16P23, "P16P23"}
};

enum OutputLayerNodeIds {
	MOVE_X,
	MOVE_Y
};

struct Node;

using Neuron = std::shared_ptr<Node>;

struct Edge {
    double weight;
	Neuron src_neuron;
	Neuron dst_neuron;
};

using Synapse = std::shared_ptr<Edge>;

struct Node {
    int id;
    double value;
	std::vector<Synapse> incoming_edges;
};

struct InputNodeValue {
    InputLayerNodeIds id;
    double value;
};

struct Layer {
	std::vector<Neuron> neurons;
};

using InputLayerValues = std::vector<InputNodeValue>;

class NeuralNetwork
{
public:
    NeuralNetwork();
	NeuralNetwork(std::ifstream& in_file);
	NeuralNetwork(const NeuralNetwork& other);
    ~NeuralNetwork();

	Point decideMovement(InputLayerValues input_layer_values);
	int getNeuronIdCounter() const;
	std::string exportGraph(std::string entity_type);
	std::string getSaveString(std::string entity_type);

private:
	void createInputAndOutputLayer();
	void setInputLayer(InputLayerValues input_layer_values);
	void calculateNeuronValue(Neuron neuron);
	Neuron getRandInputLayerNeuron();
	Neuron getRandHiddenLayerNeuron(size_t hiddenLayerIndex);
	Neuron getRandNeuronFromFollowingLayers(size_t startLayerIndex);
	void init();
	void mutate();

	std::map<InputLayerNodeIds, Neuron> m_input_layer;
	std::map<OutputLayerNodeIds, Neuron> m_output_layer;
	std::vector<Layer> m_hidden_layers;
	std::vector<Synapse> m_synapses;
	std::vector<Neuron> m_neurons;
	int m_neuron_id_counter = 0;
};

#endif /* PREYDATOR_NEURAL_NETWORK_H__ */