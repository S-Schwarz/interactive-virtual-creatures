//
// Created by st on 7/12/21.
//

#include "Neuron.h"

void ivc::Neuron::swap() {
    output.swap();
}

void ivc::Neuron::step() {

}

ivc::Gate* ivc::Neuron::getOutputGate() {
    return &output;
}

std::vector<unsigned long> ivc::Neuron::getGateIDs() {
    return {};
}

int ivc::Neuron::bindGates(std::vector<Gate *>) {
    return -1;
}

unsigned long ivc::Neuron::getOutputID() {
    return m_outputID;
}

void ivc::Neuron::setID(unsigned long id) {
    m_outputID = id;
}

void ivc::Neuron::chooseRandomInputs(std::vector<unsigned long>) {
}
