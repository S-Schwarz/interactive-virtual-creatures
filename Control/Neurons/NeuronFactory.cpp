//
// Created by st on 7/14/21.
//

#include "NeuronFactory.h"

ivc::Neuron *ivc::NeuronFactory::createRandomNeuron(std::mt19937* gen) {

    std::uniform_int_distribution<> dis(0, COUNT-1);

    auto type = static_cast<NEURON_TYPE>(dis(*gen));

    Neuron* newNeuron = nullptr;

    switch (type) {
        case SUM:
        case MAX:
        case MIN:
        case PRODUCT:
        case DIVIDE:
        case SUM_THRESHOLD:
        case GREATER_THAN:
            newNeuron = new Neuron_TwoInputs(type);
            break;
        case SIGN:
        case ABS:
        case SIN:
            newNeuron = new Neuron_OneInput(type);
            break;
        case CONSTANT:
            newNeuron = new Neuron_ZeroInputs(type);
            break;
        default:
            throw std::invalid_argument("INVALID NEURON TYPE");
    }

    newNeuron->mutate(gen,true);
    return newNeuron;

}
