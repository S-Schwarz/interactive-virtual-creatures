//
// Created by st on 7/14/21.
//

#include "NeuronFactory.h"

ivc::Neuron *ivc::NeuronFactory::createRandomNeuron(std::mt19937 gen) {

    std::uniform_int_distribution<> dis(0, COUNT-1);

    auto type = static_cast<NEURON_TYPE>(dis(gen));

    Neuron* newNeuron = nullptr;

    switch (type) {
        case SUM:
            newNeuron = new Neuron_Sum();
            newNeuron->randomize(gen);
            return newNeuron;
        case MAX:
            newNeuron = new Neuron_Max();
            newNeuron->randomize(gen);
            return newNeuron;
        case MIN:
            newNeuron = new Neuron_Min();
            newNeuron->randomize(gen);
            return newNeuron;
        case CONSTANT:
            newNeuron = new Neuron_Constant();
            newNeuron->randomize(gen);
            return newNeuron;
        default:
            return nullptr;
    }

}
