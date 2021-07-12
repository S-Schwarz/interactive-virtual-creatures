//
// Created by st on 7/12/21.
//

#include "Neuron.h"

void ivc::Neuron::swap() {
    output.swap();
}

void ivc::Neuron::step() {

}

ivc::Gate* ivc::Neuron::getGate() {
    return &output;
}