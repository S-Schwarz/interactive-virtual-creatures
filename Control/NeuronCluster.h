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
#include "Joints/JointSensor.h"
#include "Joints/JointEffector.h"
#include "IDHandler.h"

namespace ivc {

    class NeuronCluster {
    private:
        std::vector<Neuron*> m_neuronVector;
        JointSensor* m_sensor = nullptr;
        JointEffector* m_effector = nullptr;
        std::vector<unsigned long> m_outputGates;
        std::vector<unsigned long> m_possibleInputGates;

    public:
        NeuronCluster(std::mt19937,bool,bool,IDHandler*);
        std::vector<unsigned long> getOutputGates();
        void setPossibleInputs(std::vector<unsigned long>);
        void randomizeConnections();
        std::vector<Neuron*> getCopyOfNeurons();
    };

}

#endif //INTERACTIVE_VIRTUAL_CREATURES_NEURONCLUSTER_H
