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

    if(gates[0] == nullptr || gates[1] == nullptr)
        printf("TEST\n");

    input_0 = gates[0];
    input_1 = gates[1];

    return 0;
}

void ivc::Neuron_TwoInputs::chooseRandomInputs(std::vector<unsigned long> possibleInputs) {

    if(possibleInputs.empty())
        return;

    static auto rng = std::default_random_engine(std::chrono::system_clock::now().time_since_epoch().count());

    std::shuffle(std::begin(possibleInputs), std::end(possibleInputs), rng);
    id_input_0 = possibleInputs[0];

    std::shuffle(std::begin(possibleInputs), std::end(possibleInputs), rng);
    id_input_1 = possibleInputs[0];
}

void ivc::Neuron_TwoInputs::randomize(std::mt19937* gen) {
    Neuron::randomize(gen);

    std::normal_distribution<> inputDis(MEAN_NEURON_WEIGHT, MEAN_NEURON_WEIGHT * STANDARD_DEVIATION_FACTOR);
    weight_0 = inputDis(*gen);
    weight_1 = inputDis(*gen);

}

void ivc::Neuron_TwoInputs::mutate(std::mt19937 *gen) {
    Neuron::mutate(gen);
    //mutate input weights
    weight_0 = Mutator::mutateFloat(gen,weight_0, INFINITY, -INFINITY);
    weight_1 = Mutator::mutateFloat(gen,weight_1, INFINITY, -INFINITY);
}
