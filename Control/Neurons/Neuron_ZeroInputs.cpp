//
// Created by st on 7/18/21.
//

#include "Neuron_ZeroInputs.h"

ivc::Neuron_ZeroInputs::Neuron_ZeroInputs(ivc::NEURON_TYPE type) {
    m_type = type;
}

ivc::Neuron *ivc::Neuron_ZeroInputs::copy() {
    return new Neuron_ZeroInputs(*this);
}

void ivc::Neuron_ZeroInputs::step() {
    switch (m_type) {
        case CONSTANT:
            output->setValue(m_constant);
            break;
        default:
            throw std::invalid_argument("INVALID NEURON TYPE");
    }
}

void ivc::Neuron_ZeroInputs::randomize(std::mt19937 *gen) {
    Neuron::randomize(gen);

    std::normal_distribution<> constantDis(MEAN_NEURON_CONSTANT, MEAN_NEURON_CONSTANT * STANDARD_DEVIATION_FACTOR);
    m_constant = constantDis(*gen);
}

void ivc::Neuron_ZeroInputs::mutate(std::mt19937 *gen) {
    Neuron::mutate(gen);

    std::uniform_real_distribution<> dis(0, 1);

    //mutateBodyAndNeurons constant
    if(dis(*gen) <= MUTATE_CONSTANT_CHANCE)
        m_constant = Mutator::mutateFloat(gen, m_constant, INFINITY, -INFINITY);
}

void ivc::Neuron_ZeroInputs::mutateConnections(std::mt19937 *, std::vector<unsigned long>) {}
