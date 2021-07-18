//
// Created by st on 7/18/21.
//

#include "Neuron_OneInput.h"

ivc::Neuron_OneInput::Neuron_OneInput(ivc::NEURON_TYPE type) {
    m_type = type;
}

void ivc::Neuron_OneInput::randomize(std::mt19937 *gen) {
    Neuron::randomize(gen);
    mutate(gen);
}

std::vector<unsigned long> ivc::Neuron_OneInput::getGateIDs() {
    return {id_input_0};
}

int ivc::Neuron_OneInput::bindGates(std::vector<Gate *> gates) {
    if(gates.size() != 1)
        return -1;

    input_0 = gates[0];

    return 0;
}

void ivc::Neuron_OneInput::chooseRandomInputs(std::vector<unsigned long> possibleInputs) {

    if(possibleInputs.empty())
        return;

    static auto rng = std::default_random_engine(std::chrono::system_clock::now().time_since_epoch().count());

    std::shuffle(std::begin(possibleInputs), std::end(possibleInputs), rng);
    id_input_0 = possibleInputs[0];

}

void ivc::Neuron_OneInput::mutate(std::mt19937 *gen) {
    Neuron::mutate(gen);

    std::uniform_real_distribution<> dis(0, 1);

    //mutateBodyAndNeurons input weights
    if(dis(*gen) <= MUTATE_INPUT_WEIGHT_CHANCE)
        weight_0 = Mutator::mutateFloat(gen,weight_0, INFINITY, -INFINITY);

    //mutate sine params
    if(dis(*gen) <= MUTATE_SINE_CHANCE)
        m_sin_amplitude = Mutator::mutateFloat(gen,m_sin_amplitude, INFINITY, -INFINITY);
    if(dis(*gen) <= MUTATE_SINE_CHANCE)
        m_sin_period = Mutator::mutateFloat(gen,m_sin_period, INFINITY, -INFINITY);
    if(dis(*gen) <= MUTATE_SINE_CHANCE)
        m_sin_phase = Mutator::mutateFloat(gen,m_sin_phase, INFINITY, -INFINITY);
    if(dis(*gen) <= MUTATE_SINE_CHANCE)
        m_sin_vertical = Mutator::mutateFloat(gen,m_sin_vertical, INFINITY, -INFINITY);

}

void ivc::Neuron_OneInput::mutateConnections(std::mt19937 *gen, std::vector<unsigned long> possibleInputs) {
    std::uniform_real_distribution<> dis(0, 1);

    //mutateBodyAndNeurons input connections
    if(possibleInputs.empty())
        return;

    static auto rng = std::default_random_engine(std::chrono::system_clock::now().time_since_epoch().count());


    if(std::find(possibleInputs.begin(), possibleInputs.end(), id_input_0) == possibleInputs.end() || dis(*gen) <= MUTATE_CONNECTION_CHANCE){
        std::shuffle(std::begin(possibleInputs), std::end(possibleInputs), rng);
        id_input_0 = possibleInputs[0];
    }
}

ivc::Neuron *ivc::Neuron_OneInput::copy() {
    return new Neuron_OneInput(*this);
}

void ivc::Neuron_OneInput::step() {
    switch (m_type) {
        case SIGN:
            sign();
            break;
        case ABS:
            abs();
            break;
        case SIN:
            sin();
            break;
        default:
            throw std::invalid_argument("INVALID NEURON TYPE");
    }
}

void ivc::Neuron_OneInput::sign() {
    float val = weight_0 * input_0->getValue();

    if(val > 0)
        output->setValue(1.0f);
    else if(val < 0)
        output->setValue(-1.0f);
    else
        output->setValue(0.0f);
}

void ivc::Neuron_OneInput::abs() {
    float val = weight_0 * input_0->getValue();

    if(val < 0)
        output->setValue(-1.0f * val);
    else
        output->setValue(val);
}

void ivc::Neuron_OneInput::sin() {
    float val = weight_0 * input_0->getValue();
    float result = m_sin_amplitude * std::sin(m_sin_period * (val + m_sin_phase)) + m_sin_vertical;
    output->setValue(result);
}
