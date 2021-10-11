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
#include <memory>

namespace ivc {

    class NeuronCluster {
    private:
        std::vector<std::shared_ptr<Neuron>> m_neuronVector;
        std::shared_ptr<JointSensor> m_sensor = nullptr;
        std::shared_ptr<JointEffector> m_effector = nullptr;
        std::shared_ptr<ContactSensor> m_contact = nullptr;
        std::vector<unsigned long> m_outputGates;
        std::vector<unsigned long> m_possibleInputGates;
        std::shared_ptr<std::mt19937> m_generator = nullptr;

    public:
        NeuronCluster()=default;
        NeuronCluster(std::shared_ptr<std::mt19937>,bool,bool,std::shared_ptr<IDHandler>,std::shared_ptr<EvoConfig>);
        std::vector<unsigned long> getOutputGates();
        void setPossibleInputs(std::vector<unsigned long>);
        void setPossibleInputsDirectly(std::vector<unsigned long>);
        std::vector<unsigned long> getPossibleInputs();
        void setOutputGates(std::vector<unsigned long>);
        void randomizeConnections();
        std::vector<std::shared_ptr<Neuron>> getCopyOfNeurons();
        std::pair<std::shared_ptr<JointSensor>,std::shared_ptr<JointEffector>> getCopiesOfJointNeurons();
        std::shared_ptr<ContactSensor> getCopyOfContactSensor();
        void setContactSensor(std::shared_ptr<ContactSensor>);
        void mutateNeurons(std::shared_ptr<EvoConfig>);
        void mutateNewNeurons(std::shared_ptr<IDHandler>, std::shared_ptr<EvoConfig>);
        void mutateConnections(std::shared_ptr<EvoConfig>);
        std::shared_ptr<NeuronCluster> copy();
        void setNeurons(std::vector<std::shared_ptr<Neuron>>);
        void setJointNeurons(std::shared_ptr<JointSensor>,std::shared_ptr<JointEffector>);
        void setGenerator(std::shared_ptr<std::mt19937>);
        void chooseNewNeuronIDs(std::map<unsigned long,unsigned long>*,std::shared_ptr<IDHandler>);
        void rewireInputs(std::map<unsigned long,unsigned long>*);
    };

}

#endif //INTERACTIVE_VIRTUAL_CREATURES_NEURONCLUSTER_H
