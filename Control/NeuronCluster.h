//
// Created by st on 7/7/21.
//

#ifndef INTERACTIVE_VIRTUAL_CREATURES_NEURONCLUSTER_H
#define INTERACTIVE_VIRTUAL_CREATURES_NEURONCLUSTER_H

#include <vector>
#include <random>
#include "Neurons/Neuron.h"
#include "../Constants.h"
#include "Neurons/NeuronFactory.h"

namespace ivc {

    class NeuronCluster {
    private:
        std::vector<Neuron*> m_neuronVector;
    public:
        NeuronCluster(std::mt19937,bool);
    };

}

#endif //INTERACTIVE_VIRTUAL_CREATURES_NEURONCLUSTER_H
