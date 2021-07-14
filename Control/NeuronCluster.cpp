//
// Created by st on 7/7/21.
//

#include "NeuronCluster.h"

ivc::NeuronCluster::NeuronCluster(std::mt19937 gen,bool isBrain) {

    int mean = isBrain ? MEAN_BRAIN_NEURONS : MEAN_LOCAL_NEURONS;

    std::binomial_distribution<> dis(mean, STANDARD_DEVIATION_FACTOR);

    auto numberNeurons = dis(gen);

    for(int i = 0; i < numberNeurons; ++i){
        m_neuronVector.push_back(NeuronFactory::createRandomNeuron(gen));
    }

    m_sensor = new JointSensor();
    m_effector = new JointEffector();

}

std::vector<unsigned long> ivc::NeuronCluster::getOutputGates() {
    return m_outputGates;
}
