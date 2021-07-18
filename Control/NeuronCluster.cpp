//
// Created by st on 7/7/21.
//

#include "NeuronCluster.h"

ivc::NeuronCluster::NeuronCluster(std::mt19937* gen, bool isBrain,bool isRoot, IDHandler* idHandler) {

    m_generator = gen;

    int mean = isBrain ? MEAN_BRAIN_NEURONS : MEAN_LOCAL_NEURONS;

    std::binomial_distribution<> dis(mean, STANDARD_DEVIATION_FACTOR);

    auto numberNeurons = dis(*gen);

    for(int i = 0; i < numberNeurons; ++i){
        auto newNeuron = NeuronFactory::createRandomNeuron(gen);
        auto newID = idHandler->getNewID();
        newNeuron->setID(newID);
        m_outputGates.push_back(newID);
        m_neuronVector.push_back(newNeuron);
    }

    if(!isBrain && !isRoot){
        m_sensor = new JointSensor();
        m_sensor->randomize(gen);
        auto sensorID_0 = idHandler->getNewID();
        m_outputGates.push_back(sensorID_0);
        auto sensorID_1 = idHandler->getNewID();
        m_outputGates.push_back(sensorID_1);
        auto sensorID_2 = idHandler->getNewID();
        m_outputGates.push_back(sensorID_2);
        m_sensor->setIDs(sensorID_0,sensorID_1,sensorID_2);

        m_effector = new JointEffector();
        m_effector->randomize(gen);
    }

}

std::vector<unsigned long> ivc::NeuronCluster::getOutputGates() {
    return m_outputGates;
}

void ivc::NeuronCluster::setPossibleInputs(std::vector<unsigned long> vec) {
    std::vector<unsigned long> allPossible = m_outputGates;
    allPossible.insert(allPossible.end(), vec.begin(), vec.end());
    m_possibleInputGates = allPossible;
}

void ivc::NeuronCluster::randomizeConnections() {
    for(auto neuron : m_neuronVector){
        neuron->chooseRandomInputs(m_possibleInputGates);
    }

    if(m_effector != nullptr)
        m_effector->chooseRandomInputs(m_possibleInputGates);

}

std::vector<ivc::Neuron *> ivc::NeuronCluster::getCopyOfNeurons() {
    std::vector<Neuron*> copyVec;
    for(auto neuron : m_neuronVector){
        copyVec.push_back(neuron->copy());
    }
    return copyVec;
}

std::pair<ivc::JointSensor *, ivc::JointEffector *> ivc::NeuronCluster::getCopiesOfJointNeurons() {
    return {new JointSensor(*m_sensor), new JointEffector(*m_effector)};
}

void ivc::NeuronCluster::mutateNeurons(IDHandler* idHandler) {
    std::uniform_real_distribution<> dis(0, 1);

    if(dis(*m_generator) <= MUTATE_NEW_NEURON_CHANCE){
        auto newNeuron = NeuronFactory::createRandomNeuron(m_generator);
        auto newID = idHandler->getNewID();
        newNeuron->setID(newID);
        m_outputGates.push_back(newID);
        m_neuronVector.push_back(newNeuron);
    }
}

void ivc::NeuronCluster::mutateConnections() {
    for(auto neuron : m_neuronVector){
        neuron->mutate(m_generator,m_possibleInputGates);
    }
    if(m_sensor != nullptr){
        m_sensor->mutate(m_generator);
        m_effector->mutate(m_generator);
    }

}

ivc::NeuronCluster *ivc::NeuronCluster::copy() {
    auto copiedCluster = new NeuronCluster(*this);

    copiedCluster->setNeurons(getCopyOfNeurons());
    if(m_sensor != nullptr)
        copiedCluster->setJointNeurons(new JointSensor(*m_sensor), new JointEffector(*m_effector));

    return copiedCluster;
}

void ivc::NeuronCluster::setNeurons(std::vector<Neuron *> neurons) {
    m_neuronVector = neurons;
}

void ivc::NeuronCluster::setJointNeurons(ivc::JointSensor * sensor, ivc::JointEffector* effector) {
    m_sensor = sensor;
    m_effector = effector;
}

void ivc::NeuronCluster::setGenerator(std::mt19937 *gen) {
    m_generator = gen;
}
