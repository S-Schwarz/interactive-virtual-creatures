//
// Created by st on 7/14/21.
//

#include "NeuronFactory.h"

ivc::Neuron *ivc::NeuronFactory::createRandomNeuron() {

    static std::random_device rd;
    static std::mt19937 gen(rd());

    std::uniform_int_distribution<> dis(0, COUNT-1);

    auto type = static_cast<NEURON_TYPE>(dis(gen));

    switch (type) {
        case SUM:
            return new Neuron_Sum();
        case MAX:
            return new Neuron_Max();
        case MIN:
            return new Neuron_Min();
        default:
            return nullptr;
    }

}
