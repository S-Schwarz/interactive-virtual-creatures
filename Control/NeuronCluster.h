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
        std::mt19937* m_generator = nullptr;

    public:
        NeuronCluster(std::mt19937*,bool,bool,IDHandler*);
        std::vector<unsigned long> getOutputGates();
        void setPossibleInputs(std::vector<unsigned long>);
        void randomizeConnections();
        std::vector<Neuron*> getCopyOfNeurons();
        std::pair<JointSensor*,JointEffector*> getCopiesOfJointNeurons();
        void mutateNeurons(IDHandler*);
        void mutateConnections();
        NeuronCluster* copy();
        void setNeurons(std::vector<Neuron*>);
        void setJointNeurons(JointSensor*,JointEffector*);
        void setGenerator(std::mt19937*);
    };

}

#endif //INTERACTIVE_VIRTUAL_CREATURES_NEURONCLUSTER_H
