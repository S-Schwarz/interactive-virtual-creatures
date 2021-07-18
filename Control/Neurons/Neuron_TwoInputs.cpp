//
// Created by st on 7/14/21.
//

#include "Neuron_TwoInputs.h"

ivc::Neuron_TwoInputs::Neuron_TwoInputs(ivc::NEURON_TYPE type) {
    m_type = type;
}

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

void ivc::Neuron_TwoInputs::mutateConnections(std::mt19937 *gen, std::vector<unsigned long> possibleInputs) {

    std::uniform_real_distribution<> dis(0, 1);

    //mutateBodyAndNeurons input connections
    if(possibleInputs.empty())
        return;

    static auto rng = std::default_random_engine(std::chrono::system_clock::now().time_since_epoch().count());


    if(std::find(possibleInputs.begin(), possibleInputs.end(), id_input_0) == possibleInputs.end() || dis(*gen) <= MUTATE_CONNECTION_CHANCE){
        std::shuffle(std::begin(possibleInputs), std::end(possibleInputs), rng);
        id_input_0 = possibleInputs[0];
    }

    if(std::find(possibleInputs.begin(), possibleInputs.end(), id_input_1) == possibleInputs.end() || dis(*gen) <= MUTATE_CONNECTION_CHANCE){
        std::shuffle(std::begin(possibleInputs), std::end(possibleInputs), rng);
        id_input_1 = possibleInputs[0];
    }
}

void ivc::Neuron_TwoInputs::mutate(std::mt19937 *gen, bool forceMutation) {
    Neuron::mutate(gen,forceMutation);

    std::uniform_real_distribution<> dis(0, 1);

    //mutateBodyAndNeurons input weights
    if(forceMutation || dis(*gen) <= MUTATE_INPUT_WEIGHT_CHANCE)
        weight_0 = Mutator::mutateFloat(gen,weight_0, INFINITY, -INFINITY);
    if(forceMutation || dis(*gen) <= MUTATE_INPUT_WEIGHT_CHANCE)
        weight_1 = Mutator::mutateFloat(gen,weight_1, INFINITY, -INFINITY);

    //mutate sum_threshold threshold
    if(forceMutation || dis(*gen) <= MUTATE_THRESHOLD_CHANCE)
        m_threshold = Mutator::mutateFloat(gen,m_threshold, INFINITY, -INFINITY);

}

ivc::Neuron *ivc::Neuron_TwoInputs::copy() {
    return new Neuron_TwoInputs(*this);
}

void ivc::Neuron_TwoInputs::step() {
    switch (m_type) {
        case SUM:
            sum();
            break;
        case MIN:
            min();
            break;
        case MAX:
            max();
            break;
        case PRODUCT:
            product();
            break;
        case DIVIDE:
            divide();
            break;
        case SUM_THRESHOLD:
            sum_threshold();
            break;
        case GREATER_THAN:
            greater_than();
            break;
        default:
            throw std::invalid_argument("INVALID NEURON TYPE");
    }
}

void ivc::Neuron_TwoInputs::sum() {
    output->setValue(m_outputWeight * (weight_0 * input_0->getValue() + weight_1 * input_1->getValue()));
}

void ivc::Neuron_TwoInputs::min() {
    float val_0 = weight_0 * input_0->getValue();
    float val_1 = weight_1 * input_1->getValue();

    if(val_0 < val_1)
        output->setValue(m_outputWeight * val_0);
    else
        output->setValue(m_outputWeight * val_1);
}

void ivc::Neuron_TwoInputs::max() {
    float val_0 = weight_0 * input_0->getValue();
    float val_1 = weight_1 * input_1->getValue();

    if(val_0 > val_1)
        output->setValue(m_outputWeight * val_0);
    else
        output->setValue(m_outputWeight * val_1);
}

void ivc::Neuron_TwoInputs::product() {
    float val_0 = weight_0 * input_0->getValue();
    float val_1 = weight_1 * input_1->getValue();

    output->setValue(m_outputWeight * val_0 * val_1);
}

void ivc::Neuron_TwoInputs::divide() {
    float val_0 = weight_0 * input_0->getValue();
    float val_1 = weight_1 * input_1->getValue();

    if(val_1 == 0)
        val_1 = 0.00001;

    output->setValue(m_outputWeight * (val_0 / val_1));
}

void ivc::Neuron_TwoInputs::sum_threshold() {
    float val_0 = weight_0 * input_0->getValue();
    float val_1 = weight_1 * input_1->getValue();

    float sum = val_0 + val_1;

    if(sum >= m_threshold)
        output->setValue(1.0f);
    else
        output->setValue(0.0f);

}

void ivc::Neuron_TwoInputs::greater_than() {
    float val_0 = weight_0 * input_0->getValue();
    float val_1 = weight_1 * input_1->getValue();

    if(val_0 > val_1)
        output->setValue(1.0f);
    else
        output->setValue(0.0f);

}
