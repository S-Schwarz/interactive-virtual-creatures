//
// Created by st on 7/7/21.
//

#include "NeuronCluster.h"

ivc::NeuronCluster::NeuronCluster(std::shared_ptr<std::mt19937> gen, bool isBrain,bool isRoot, std::shared_ptr<IDHandler> idHandler, std::shared_ptr<EvoConfig> config) {

    m_generator = gen;

    int mean = isBrain ? MEAN_BRAIN_NEURONS : MEAN_LOCAL_NEURONS;

    std::binomial_distribution<> dis(mean, STANDARD_DEVIATION_FACTOR);

    auto numberNeurons = mean;

    for(int i = 0; i < numberNeurons; ++i){
        auto newNeuron = std::make_shared<Neuron>(m_generator, config);
        auto newID = idHandler->getNewID();
        newNeuron->setID(newID);
        m_outputGates.push_back(newID);
        m_neuronVector.push_back(newNeuron);
    }

    if(!isBrain){
        m_contact = std::make_shared<ContactSensor>();
        std::vector<unsigned long> contactIDs;
        for(int i = 0; i < 6; ++i){
            auto newID = idHandler->getNewID();
            m_outputGates.push_back(newID);
            contactIDs.push_back(newID);
        }
        m_contact->setIDs(contactIDs);
        if(!isRoot){
            m_sensor = std::make_shared<JointSensor>();
            auto sensorID = idHandler->getNewID();
            m_outputGates.push_back(sensorID);
            m_sensor->setID(sensorID);

            m_effector = std::make_shared<JointEffector>();
        }
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

std::vector<std::shared_ptr<ivc::Neuron >> ivc::NeuronCluster::getCopyOfNeurons() {
    std::vector<std::shared_ptr<Neuron>> copyVec;
    for(auto neuron : m_neuronVector){
        copyVec.push_back(neuron->copy());
    }
    return copyVec;
}

std::pair<std::shared_ptr<ivc::JointSensor >, std::shared_ptr<ivc::JointEffector >> ivc::NeuronCluster::getCopiesOfJointNeurons() {
    return {std::make_shared<JointSensor>(*m_sensor), std::make_shared<JointEffector>(*m_effector)};
}

void ivc::NeuronCluster::mutateNeurons(std::shared_ptr<EvoConfig> config) {
    for(auto neuron : m_neuronVector){
        neuron->mutate(m_generator,false,config);
    }
    if(m_effector != nullptr){
        m_effector->mutate(m_generator,config);
    }
}

void ivc::NeuronCluster::mutateNewNeurons(std::shared_ptr<ivc::IDHandler> idHandler, std::shared_ptr<EvoConfig> config) {
    std::uniform_real_distribution<> dis(0, 1);

    //add remove neuron
    if(!m_neuronVector.empty() && dis(*m_generator) <= config->m_mutChance){
        std::uniform_int_distribution<> remDis(0, m_neuronVector.size()-1);

        auto index = remDis(*m_generator);
        auto neuron = m_neuronVector[index];
        auto outputID = neuron->getOutputID();

        m_neuronVector.erase(m_neuronVector.begin() + index);
        // TODO: delete neuron ptr
        m_outputGates.erase(std::remove(m_outputGates.begin(), m_outputGates.end(), outputID), m_outputGates.end());

    }

    //add new neuron
    if(dis(*m_generator) <= config->m_mutChance){
        auto newNeuron = std::make_shared<Neuron>(m_generator,config);
        auto newID = idHandler->getNewID();
        newNeuron->setID(newID);
        m_outputGates.push_back(newID);
        m_neuronVector.push_back(newNeuron);
    }

}

void ivc::NeuronCluster::mutateConnections(std::shared_ptr<EvoConfig> config) {
    for(auto neuron : m_neuronVector){
        neuron->mutateConnections(m_generator,m_possibleInputGates,config);
    }
    if(m_sensor != nullptr){
        m_effector->mutateConnections(m_generator,m_possibleInputGates,config);
    }
}

std::shared_ptr<ivc::NeuronCluster> ivc::NeuronCluster::copy() {
    auto copiedCluster = std::make_shared<NeuronCluster>(*this);

    copiedCluster->setNeurons(getCopyOfNeurons());
    if(m_sensor != nullptr){
        copiedCluster->setJointNeurons(std::make_shared<JointSensor>(*m_sensor), std::make_shared<JointEffector>(*m_effector));
    }
    if(m_contact != nullptr){
        copiedCluster->setContactSensor(std::make_shared<ContactSensor>(*m_contact));
    }

    return copiedCluster;
}

void ivc::NeuronCluster::setNeurons(std::vector<std::shared_ptr<Neuron >> neurons) {
    m_neuronVector = neurons;
}

void ivc::NeuronCluster::setJointNeurons(std::shared_ptr<ivc::JointSensor > sensor, std::shared_ptr<ivc::JointEffector> effector) {
    m_sensor = sensor;
    m_effector = effector;
}

void ivc::NeuronCluster::setGenerator(std::shared_ptr<std::mt19937> gen) {
    m_generator = gen;
}

std::shared_ptr<ivc::ContactSensor> ivc::NeuronCluster::getCopyOfContactSensor() {
    return std::make_shared<ContactSensor>(*m_contact);
}

void ivc::NeuronCluster::setContactSensor(std::shared_ptr<ContactSensor> sensor) {
    m_contact = sensor;
}

void ivc::NeuronCluster::chooseNewNeuronIDs(std::map<unsigned long, unsigned long>* map, std::shared_ptr<IDHandler> idHandler) {

    //new IDs for sensors
    auto oldSensorID = m_sensor->getOutputID();
    std::vector<unsigned long> newSensorIDs;
    auto newSensorID = idHandler->getNewID();
    newSensorIDs.push_back(newSensorID);
    (*map)[oldSensorID] = newSensorID;
    m_sensor->setID(newSensorID);

    //new IDs for contacts
    auto oldContactIDs = m_contact->getOutputIDs();
    std::vector<unsigned long> newContactIDs;
    for(auto id : oldContactIDs){
        auto newID = idHandler->getNewID();
        newContactIDs.push_back(newID);
        (*map)[id] = newID;
    }
    m_contact->setIDs(newContactIDs);

    //new IDs for neurons
    std::vector<unsigned long> newNeuronIDs;
    for(auto neuron : m_neuronVector){
        auto oldID = neuron->getOutputID();
        auto newID = idHandler->getNewID();
        newNeuronIDs.push_back(newID);
        neuron->setID(newID);
        (*map)[oldID] = newID;
    }

    //update outputGateVec
    std::vector<unsigned long> newOutputIDs = newSensorIDs;
    newOutputIDs.insert(newOutputIDs.end(), newContactIDs.begin(), newContactIDs.end());
    newOutputIDs.insert(newOutputIDs.end(), newNeuronIDs.begin(), newNeuronIDs.end());

    m_outputGates = newOutputIDs;


}

void ivc::NeuronCluster::rewireInputs(std::map<unsigned long, unsigned long>* map) {

    //set new effector inputs
    auto oldEffectorIDs = m_effector->getInputs();
    std::vector<unsigned long> newEffectorIDs;
    for(auto id : oldEffectorIDs){
        if(map->find(id) != map->end()){
            newEffectorIDs.push_back((*map)[id]);
        }else{
            newEffectorIDs.push_back(id);
        }
    }
    m_effector->setInputs(newEffectorIDs);

    //set new neuron inputs
    for(auto neuron : m_neuronVector){
        auto oldNeuronIDs = neuron->getInputs();
        std::vector<unsigned long> newNeuronIDs;
        for(auto id : oldNeuronIDs){
            if(map->find(id) != map->end()){
                newNeuronIDs.push_back((*map)[id]);
            }else{
                newNeuronIDs.push_back(id);
            }
        }
        neuron->setInputs(newNeuronIDs);
    }

}
