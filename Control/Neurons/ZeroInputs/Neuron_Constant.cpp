//
// Created by st on 7/15/21.
//

#include "Neuron_Constant.h"

ivc::Neuron *ivc::Neuron_Constant::copy() {
    return new Neuron_Constant(*this);
}

void ivc::Neuron_Constant::step() {
    output.setValue(m_constant);
}
