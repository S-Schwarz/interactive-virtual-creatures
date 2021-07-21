//
// Created by st on 7/12/21.
//

#include "Neuron.h"

void ivc::Neuron::swap() {
    output->swap();
}

void ivc::Neuron::step() {
    switch (m_type) {
        //no input
        case CONSTANT:
            output->setValue(m_constant);
            break;
        //one input
        case SIGN:
            sign();
            break;
        case ABS:
            abs();
            break;
        case SIN:
            sin();
            break;
        case SINE_OSCI:
            sine_osci();
            break;
        //two inputs
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

ivc::Gate* ivc::Neuron::getOutputGate() {
    return output;
}

std::vector<unsigned long> ivc::Neuron::getGateIDs() {
    return m_inputIDs;
}

int ivc::Neuron::bindGates(std::vector<Gate *> gates) {
    if(gates.size() != m_numberInputs)
        return -1;

    m_inputGates = gates;

    return 0;
}

unsigned long ivc::Neuron::getOutputID() {
    return m_outputID;
}

void ivc::Neuron::setID(unsigned long id) {
    m_outputID = id;
}

void ivc::Neuron::chooseRandomInputs(std::vector<unsigned long> possibleInputs) {
    if(possibleInputs.empty())
        return;

    static auto rng = std::default_random_engine(std::chrono::system_clock::now().time_since_epoch().count());

    for(int i = 0; i < m_numberInputs; ++i){
        std::shuffle(std::begin(possibleInputs), std::end(possibleInputs), rng);
        m_inputIDs[i] = possibleInputs[0];
    }

}

ivc::Neuron* ivc::Neuron::copy() {
    return new Neuron(*this);
}

void ivc::Neuron::setOutput(ivc::Gate * gate) {
    output = gate;
}

void ivc::Neuron::mutate(std::mt19937* gen, bool forceMutation) {

    std::uniform_real_distribution<> dis(0, 1);

    //mutate outputWeight
    if(forceMutation || dis(*gen) <= MUTATE_OUTPUT_WEIGHT_CHANCE)
        m_outputWeight = Mutator::mutateFloat(gen, m_outputWeight, INFINITY, -INFINITY);

    //mutate constant
    if(forceMutation || dis(*gen) <= MUTATE_CONSTANT_CHANCE)
        m_constant = Mutator::mutateFloat(gen, m_constant, INFINITY, -INFINITY);

    //mutate input weights
    for(int i = 0; i < m_numberInputs; ++i){
        if(forceMutation || dis(*gen) <= MUTATE_INPUT_WEIGHT_CHANCE)
            m_inputWeights[i] = Mutator::mutateFloat(gen,m_inputWeights[i], INFINITY, -INFINITY);
    }

    //mutate sine params
    if(forceMutation || dis(*gen) <= MUTATE_SINE_CHANCE)
        m_sin_amplitude = Mutator::mutateFloat(gen,m_sin_amplitude, INFINITY, -INFINITY);
    if(forceMutation || dis(*gen) <= MUTATE_SINE_CHANCE)
        m_sin_period = Mutator::mutateFloat(gen,m_sin_period, INFINITY, -INFINITY);
    if(forceMutation || dis(*gen) <= MUTATE_SINE_CHANCE)
        m_sin_phase = Mutator::mutateFloat(gen,m_sin_phase, INFINITY, -INFINITY);
    if(forceMutation || dis(*gen) <= MUTATE_SINE_CHANCE)
        m_sin_vertical = Mutator::mutateFloat(gen,m_sin_vertical, INFINITY, -INFINITY);

    //mutate sum_threshold threshold
    if(forceMutation || dis(*gen) <= MUTATE_THRESHOLD_CHANCE)
        m_threshold = Mutator::mutateFloat(gen,m_threshold, INFINITY, -INFINITY);

}

ivc::Neuron::Neuron(std::mt19937* gen) {

    std::uniform_int_distribution<> dis(0, COUNT-1);
    m_type = static_cast<NEURON_TYPE>(dis(*gen));

    switch (m_type){
        case CONSTANT:
            m_numberInputs = 0;
            break;
        case SIGN:
        case ABS:
        case SIN:
        case SINE_OSCI:
            m_numberInputs = 1;
            break;
        case SUM:
        case MAX:
        case MIN:
        case PRODUCT:
        case DIVIDE:
        case SUM_THRESHOLD:
        case GREATER_THAN:
            m_numberInputs = 2;
            break;
        default:
            throw std::invalid_argument("INVALID NEURON TYPE");
    }

    for(int i = 0; i < m_numberInputs; ++i){
        m_inputIDs.push_back(-1);
        m_inputWeights.push_back(MEAN_NEURON_WEIGHT);
    }

    mutate(gen, true);
}

void ivc::Neuron::mutateConnections(std::mt19937 *gen, std::vector<unsigned long> possibleInputs) {
    std::uniform_real_distribution<> dis(0, 1);

    //mutate input connections
    if(possibleInputs.empty())
        return;

    static auto rng = std::default_random_engine(std::chrono::system_clock::now().time_since_epoch().count());

    for(int i = 0; i < m_numberInputs; ++i){
        if(std::find(possibleInputs.begin(), possibleInputs.end(), m_inputIDs[i]) == possibleInputs.end() || dis(*gen) <= MUTATE_CONNECTION_CHANCE){
            std::shuffle(std::begin(possibleInputs), std::end(possibleInputs), rng);
            m_inputIDs[i] = possibleInputs[0];
        }
    }

}

void ivc::Neuron::sign() {
    float val = m_inputWeights[0] * m_inputGates[0]->getValue();

    if(val > 0)
        output->setValue(1.0f);
    else if(val < 0)
        output->setValue(-1.0f);
    else
        output->setValue(0.0f);
}

void ivc::Neuron::abs() {
    float val = m_inputWeights[0] * m_inputGates[0]->getValue();

    if(val < 0)
        output->setValue(-1.0f * val);
    else
        output->setValue(val);
}

void ivc::Neuron::sin() {
    float val = m_inputWeights[0] * m_inputGates[0]->getValue();
    float result = m_sin_amplitude * std::sin(m_sin_period * (val + m_sin_phase)) + m_sin_vertical;
    output->setValue(result);
}

void ivc::Neuron::sine_osci() {
    float val = m_inputWeights[0] * m_inputGates[0]->getValue();
    float result = m_sin_amplitude * std::sin(m_sin_period * (val + m_osci_offset)) + m_sin_vertical;
    m_osci_offset += m_osci_stepSize;
    output->setValue(result);
}

void ivc::Neuron::sum() {
    output->setValue(m_outputWeight * (m_inputWeights[0] * m_inputGates[0]->getValue() + m_inputWeights[1] * m_inputGates[1]->getValue()));
}

void ivc::Neuron::min() {
    float val_0 = m_inputWeights[0] * m_inputGates[0]->getValue();
    float val_1 = m_inputWeights[1] * m_inputGates[1]->getValue();

    if(val_0 < val_1)
        output->setValue(m_outputWeight * val_0);
    else
        output->setValue(m_outputWeight * val_1);
}

void ivc::Neuron::max() {
    float val_0 = m_inputWeights[0] * m_inputGates[0]->getValue();
    float val_1 = m_inputWeights[1] * m_inputGates[1]->getValue();

    if(val_0 > val_1)
        output->setValue(m_outputWeight * val_0);
    else
        output->setValue(m_outputWeight * val_1);
}

void ivc::Neuron::product() {
    float val_0 = m_inputWeights[0] * m_inputGates[0]->getValue();
    float val_1 = m_inputWeights[1] * m_inputGates[1]->getValue();

    output->setValue(m_outputWeight * val_0 * val_1);
}

void ivc::Neuron::divide() {
    float val_0 = m_inputWeights[0] * m_inputGates[0]->getValue();
    float val_1 = m_inputWeights[1] * m_inputGates[1]->getValue();

    if(val_1 == 0)
        val_1 = 0.00001;

    output->setValue(m_outputWeight * (val_0 / val_1));
}

void ivc::Neuron::sum_threshold() {
    float val_0 = m_inputWeights[0] * m_inputGates[0]->getValue();
    float val_1 = m_inputWeights[1] * m_inputGates[1]->getValue();

    float sum = val_0 + val_1;

    if(sum >= m_threshold)
        output->setValue(1.0f);
    else
        output->setValue(0.0f);

}

void ivc::Neuron::greater_than() {
    float val_0 = m_inputWeights[0] * m_inputGates[0]->getValue();
    float val_1 = m_inputWeights[1] * m_inputGates[1]->getValue();

    if(val_0 > val_1)
        output->setValue(1.0f);
    else
        output->setValue(0.0f);

}
