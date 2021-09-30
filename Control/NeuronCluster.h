//
// Created by st on 7/7/21.
//

#ifndef INTERACTIVE_VIRTUAL_CREATURES_NEURONCLUSTER_H
#define INTERACTIVE_VIRTUAL_CREATURES_NEURONCLUSTER_H

#include <vector>
#include <random>
#include "Neuron.h"
#include "../Constants.h"
#include "Joints/JointSensor.h"
#include "Joints/JointEffector.h"
#include "IDHandler.h"
#include "ContactSensor.h"
#include "../Evolution/EvoConfig.h"
#include <map>

namespace ivc {

    class NeuronCluster {
    private:
        std::vector<Neuron*> m_neuronVector;
        JointSensor* m_sensor = nullptr;
        JointEffector* m_effector = nullptr;
        ContactSensor* m_contact = nullptr;
        std::vector<unsigned long> m_outputGates;
        std::vector<unsigned long> m_possibleInputGates;
        std::mt19937* m_generator = nullptr;

    public:
        ~NeuronCluster();
        NeuronCluster(std::mt19937*,bool,bool,IDHandler*,EvoConfig*);
        std::vector<unsigned long> getOutputGates();
        void setPossibleInputs(std::vector<unsigned long>);
        void randomizeConnections();
        std::vector<Neuron*> getCopyOfNeurons();
        std::pair<JointSensor*,JointEffector*> getCopiesOfJointNeurons();
        ContactSensor* getCopyOfContactSensor();
        void setContactSensor(ContactSensor*);
        void mutateNeurons(EvoConfig*);
        void mutateNewNeurons(IDHandler*, EvoConfig*);
        void mutateConnections(EvoConfig*);
        NeuronCluster* copy();
        void setNeurons(std::vector<Neuron*>);
        void setJointNeurons(JointSensor*,JointEffector*);
        void setGenerator(std::mt19937*);
        void chooseNewNeuronIDs(std::map<unsigned long,unsigned long>*,IDHandler*);
        void rewireInputs(std::map<unsigned long,unsigned long>*);
    };

}

#endif //INTERACTIVE_VIRTUAL_CREATURES_NEURONCLUSTER_H
