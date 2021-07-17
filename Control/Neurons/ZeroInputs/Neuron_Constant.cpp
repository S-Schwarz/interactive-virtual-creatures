//
// Created by st on 7/15/21.
//

#include "Neuron_Constant.h"

ivc::Neuron *ivc::Neuron_Constant::copy() {
    return new Neuron_Constant(*this);
}

void ivc::Neuron_Constant::step() {
    output->setValue(m_constant);
}

void ivc::Neuron_Constant::randomize(std::mt19937* gen) {
    Neuron::randomize(gen);

    std::normal_distribution<> constantDis(MEAN_NEURON_CONSTANT, MEAN_NEURON_CONSTANT * STANDARD_DEVIATION_FACTOR);
    m_constant = constantDis(*gen);
}

void ivc::Neuron_Constant::mutate(std::mt19937 *gen,std::vector<unsigned long> possibleInputs) {
    Neuron::mutate(gen,possibleInputs);

    std::uniform_real_distribution<> dis(0, 1);

    //mutate constant
    if(dis(*gen) <= MUTATE_CONSTANT_CHANCE)
        m_constant = Mutator::mutateFloat(gen, m_constant, INFINITY, -INFINITY);
}
