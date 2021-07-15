//
// Created by st on 7/12/21.
//

#include "Neuron_Sum.h"

void ivc::Neuron_Sum::step() {
    sum();
}

void ivc::Neuron_Sum::sum() {
    output->setValue(m_outputWeight * (weight_0 * input_0->getValue() + weight_1 * input_1->getValue()));
}

ivc::Neuron *ivc::Neuron_Sum::copy() {
    return new Neuron_Sum(*this);
}
