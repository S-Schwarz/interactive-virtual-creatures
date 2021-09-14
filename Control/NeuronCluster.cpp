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
        auto newNeuron = new Neuron(m_generator);
        auto newID = idHandler->getNewID();
        newNeuron->setID(newID);
        m_outputGates.push_back(newID);
        m_neuronVector.push_back(newNeuron);
    }

    if(!isBrain){
        m_contact = new ContactSensor();
        std::vector<unsigned long> contactIDs;
        for(int i = 0; i < 6; ++i){
            auto newID = idHandler->getNewID();
            m_outputGates.push_back(newID);
            contactIDs.push_back(newID);
        }
        m_contact->setIDs(contactIDs);
        if(!isRoot){
            m_sensor = new JointSensor();
            auto sensorID_0 = idHandler->getNewID();
            m_outputGates.push_back(sensorID_0);
            auto sensorID_1 = idHandler->getNewID();
            m_outputGates.push_back(sensorID_1);
            auto sensorID_2 = idHandler->getNewID();
            m_outputGates.push_back(sensorID_2);
            m_sensor->setIDs(sensorID_0,sensorID_1,sensorID_2);

            m_effector = new JointEffector();
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

void ivc::NeuronCluster::mutateNeurons() {
    for(auto neuron : m_neuronVector){
        neuron->mutate(m_generator,false);
    }
    if(m_sensor != nullptr){
        m_sensor->mutate(m_generator);
        m_effector->mutate(m_generator);
    }
}

void ivc::NeuronCluster::mutateNewNeurons(ivc::IDHandler *idHandler) {
    std::uniform_real_distribution<> dis(0, 1);

    //add remove neuron
    if(!m_neuronVector.empty() && dis(*m_generator) <= MUTATE_REMOVE_NEURON_CHANCE){
        std::uniform_int_distribution<> remDis(0, m_neuronVector.size()-1);

        auto index = remDis(*m_generator);
        auto neuron = m_neuronVector[index];
        auto outputID = neuron->getOutputID();

        m_neuronVector.erase(m_neuronVector.begin() + index);
        // TODO: delete neuron ptr
        m_outputGates.erase(std::remove(m_outputGates.begin(), m_outputGates.end(), outputID), m_outputGates.end());

    }

    //add new neuron
    if(dis(*m_generator) <= MUTATE_NEW_NEURON_CHANCE){
        auto newNeuron = new Neuron(m_generator);
        auto newID = idHandler->getNewID();
        newNeuron->setID(newID);
        m_outputGates.push_back(newID);
        m_neuronVector.push_back(newNeuron);
    }

}

void ivc::NeuronCluster::mutateConnections() {
    for(auto neuron : m_neuronVector){
        neuron->mutateConnections(m_generator,m_possibleInputGates);
    }
    if(m_sensor != nullptr){
        m_effector->mutateConnections(m_generator,m_possibleInputGates);
    }
}

ivc::NeuronCluster *ivc::NeuronCluster::copy() {
    auto copiedCluster = new NeuronCluster(*this);

    copiedCluster->setNeurons(getCopyOfNeurons());
    if(m_sensor != nullptr){
        copiedCluster->setJointNeurons(new JointSensor(*m_sensor), new JointEffector(*m_effector));
    }
    if(m_contact != nullptr){
        copiedCluster->setContactSensor(new ContactSensor(*m_contact));
    }

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

ivc::ContactSensor *ivc::NeuronCluster::getCopyOfContactSensor() {
    return new ContactSensor(*m_contact);
}

void ivc::NeuronCluster::setContactSensor(ContactSensor* sensor) {
    m_contact = sensor;
}

ivc::NeuronCluster::~NeuronCluster() {

    delete m_sensor;
    delete m_effector;
    delete m_contact;

    for(auto neuron : m_neuronVector){
        delete neuron;
    }

}

void ivc::NeuronCluster::chooseNewNeuronIDs(std::map<unsigned long, unsigned long>* map,IDHandler* idHandler) {

    //new IDs for sensors
    auto oldSensorIDs = m_sensor->getOutputIDs();
    std::vector<unsigned long> newSensorIDs;
    for(auto id : oldSensorIDs){
        auto newID = idHandler->getNewID();
        newSensorIDs.push_back(newID);
        (*map)[id] = newID;
    }
    m_sensor->setIDs(newSensorIDs);

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
