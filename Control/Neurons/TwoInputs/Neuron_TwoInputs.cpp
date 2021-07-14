//
// Created by st on 7/14/21.
//

#include "Neuron_TwoInputs.h"

std::vector<unsigned long> ivc::Neuron_TwoInputs::getGateIDs() {
    return {id_input_0,id_input_1};
}

int ivc::Neuron_TwoInputs::bindGates(std::vector<Gate *> gates) {

    if(gates.size() != 2)
        return -1;

    input_0 = gates[0];
    input_1 = gates[1];

    return 0;
}