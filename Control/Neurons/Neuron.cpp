//
// Created by st on 7/12/21.
//

#include "Neuron.h"

void ivc::Neuron::swap() {
    output->swap();
}

void ivc::Neuron::step() {

}

ivc::Gate* ivc::Neuron::getOutputGate() {
    return output;
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

ivc::Neuron* ivc::Neuron::copy() {
    return new Neuron(*this);
}

void ivc::Neuron::randomize(std::mt19937* gen) {

    std::normal_distribution<> outputDis(MEAN_NEURON_WEIGHT, MEAN_NEURON_WEIGHT * STANDARD_DEVIATION_FACTOR);
    m_outputWeight = outputDis(*gen);

}

void ivc::Neuron::setOutput(ivc::Gate * gate) {
    output = gate;
}

void ivc::Neuron::mutate(std::mt19937* gen,std::vector<unsigned long> possibleInputs) {

    std::uniform_real_distribution<> dis(0, 1);

    //mutateBodyAndNeurons outputWeight
    if(dis(*gen) <= MUTATE_OUTPUT_WEIGHT_CHANCE)
        m_outputWeight = Mutator::mutateFloat(gen, m_outputWeight, INFINITY, -INFINITY);

}
