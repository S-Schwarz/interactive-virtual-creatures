//
// Created by st on 7/12/21.
//

#include "Neuron.h"

void ivc::Neuron::swap() {
    m_output->swap();
}

void ivc::Neuron::step() {
    switch (m_type) {
        //no input
        case CONSTANT:
            m_output->setValue(m_constant);
            break;
        case SINE_OSCI:
            sine_osci();
            break;
        case SAW_OSCI:
            saw_osci();
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
        case SAW:
            saw();
            break;
        case DELAY:
            delay();
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
        //three inputs
        case IF_THEN_ELSE:
            if_then_else();
            break;
        case SIGMOID:
            sigmoid();
            break;
        case TANH:
            tanh();
            break;
        default:
            throw std::invalid_argument("INVALID NEURON TYPE");
    }
}

std::shared_ptr<ivc::Gate> ivc::Neuron::getOutputGate() {
    return m_output;
}

std::vector<unsigned long> ivc::Neuron::getGateIDs() {
    return m_inputIDs;
}

int ivc::Neuron::bindGates(std::vector<std::shared_ptr<Gate >> gates) {
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

std::shared_ptr<ivc::Neuron> ivc::Neuron::copy() {
    return std::make_shared<Neuron>(*this);
}

void ivc::Neuron::setOutput(std::shared_ptr<ivc::Gate> gate) {
    m_output = gate;
}

void ivc::Neuron::mutate(std::shared_ptr<std::mt19937> gen, bool forceMutation, std::shared_ptr<EvoConfig> config) {

    std::uniform_real_distribution<> dis(0, 1);

    //mutate constant
    if(forceMutation || dis(*gen) <= config->m_mutChance)
        m_constant = Mutator::mutateFloat(gen, m_constant, 1.0f, -1.0f);

    //mutate input weights
    for(int i = 0; i < m_numberInputs; ++i){
        if(forceMutation || dis(*gen) <= config->m_mutChance)
            m_inputWeights[i] = Mutator::mutateFloat(gen,m_inputWeights[i], 1.0f, -1.0f);
    }

    //mutate sine params
    if(forceMutation || dis(*gen) <= config->m_mutChance)
        m_amplitude = Mutator::mutateFloat(gen, m_amplitude, INFINITY, -INFINITY);
    if(forceMutation || dis(*gen) <= config->m_mutChance)
        m_period = Mutator::mutateFloat(gen, m_period, INFINITY, -INFINITY);
    if(forceMutation || dis(*gen) <= config->m_mutChance)
        m_phase = Mutator::mutateFloat(gen, m_phase, INFINITY, -INFINITY);
    if(forceMutation || dis(*gen) <= config->m_mutChance)
        m_vertical = Mutator::mutateFloat(gen, m_vertical, INFINITY, -INFINITY);

    //mutate sine osci step
    if(forceMutation || dis(*gen) <= config->m_mutChance)
        m_osci_stepSize = Mutator::mutateFloat(gen,m_osci_stepSize, INFINITY, -INFINITY);

    //mutate sum_threshold threshold
    if(forceMutation || dis(*gen) <= config->m_mutChance)
        m_threshold = Mutator::mutateFloat(gen,m_threshold, INFINITY, -INFINITY);

    //mutate general param
    if(forceMutation || dis(*gen) <= config->m_mutChance)
        m_genParam = Mutator::mutateFloat(gen, m_genParam, 3.f / 2.f, -3.f / 2.f);

}

ivc::Neuron::Neuron(std::shared_ptr<std::mt19937> gen, std::shared_ptr<EvoConfig> config) {

    if(config->m_useGeneralNeurons){
        std::uniform_real_distribution<> dis(0, 1);
        auto val = dis(*gen);
        if(val < 0.4f){
            m_type = SIGMOID;
        }else if(val < 0.8f){
            m_type = TANH;
        }else{
            m_type = CONSTANT;
        }
    }else{
        std::uniform_int_distribution<> dis(0, COUNT-1);
        m_type = static_cast<NEURON_TYPE>(dis(*gen));
    }


    switch (m_type){
        case CONSTANT:
        case SINE_OSCI:
        case SAW_OSCI:
            m_numberInputs = 0;
            break;
        case SIGN:
        case ABS:
        case SIN:
        case SAW:
        case DELAY:
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
        case IF_THEN_ELSE:
        case SIGMOID:
        case TANH:
            m_numberInputs = 3;
            break;
        default:
            throw std::invalid_argument("INVALID NEURON TYPE");
    }

    for(int i = 0; i < m_numberInputs; ++i){
        m_inputIDs.push_back(-1);
        m_inputWeights.push_back(1.0f);
    }

    mutate(gen, true, config);
}

void ivc::Neuron::mutateConnections(std::shared_ptr<std::mt19937> gen, std::vector<unsigned long> possibleInputs, std::shared_ptr<EvoConfig> config) {
    std::uniform_real_distribution<> dis(0, 1);

    //mutate input connections
    if(possibleInputs.empty())
        return;

    static auto rng = std::default_random_engine(std::chrono::system_clock::now().time_since_epoch().count());

    for(int i = 0; i < m_numberInputs; ++i){
        if(std::find(possibleInputs.begin(), possibleInputs.end(), m_inputIDs[i]) == possibleInputs.end() || dis(*gen) <= config->m_mutChance){
            std::shuffle(std::begin(possibleInputs), std::end(possibleInputs), rng);
            m_inputIDs[i] = possibleInputs[0];
        }
    }

}

void ivc::Neuron::sign() {
    float val = m_inputWeights[0] * m_inputGates[0]->getValue();

    if(val > 0)
        m_output->setValue(1.0f);
    else if(val < 0)
        m_output->setValue(-1.0f);
    else
        m_output->setValue(0.0f);
}

void ivc::Neuron::abs() {
    float val = m_inputWeights[0] * m_inputGates[0]->getValue();

    if(val < 0)
        m_output->setValue(-1.0f * val);
    else
        m_output->setValue(val);
}

void ivc::Neuron::saw(){
    float val = m_inputWeights[0] * m_inputGates[0]->getValue();
    float result = -(2*m_amplitude/M_PI) * std::atan(1 / std::tan((M_PI * (val + m_phase)) / m_period )) + m_vertical;
    result = std::clamp(result, -1.0f, 1.0f);
    m_osci_offset += m_osci_stepSize;
    m_output->setValue(result);
}

void ivc::Neuron::sin() {
    float val = m_inputWeights[0] * m_inputGates[0]->getValue();
    float result = m_amplitude * std::sin(m_period * (val + m_phase)) + m_vertical;
    result = std::clamp(result, -1.0f, 1.0f);
    m_output->setValue(result);
}

void ivc::Neuron::saw_osci(){
    float result = -(2*m_amplitude/M_PI) * std::atan(1 / std::tan((M_PI * m_osci_offset) / m_period )) + m_vertical;
    result = std::clamp(result, -1.0f, 1.0f);
    m_osci_offset += m_osci_stepSize;
    m_output->setValue(result);
}

void ivc::Neuron::sine_osci() {
    float result = m_amplitude * std::sin(m_period * m_osci_offset) + m_vertical;
    result = std::clamp(result, -1.0f, 1.0f);
    m_osci_offset += m_osci_stepSize;
    m_output->setValue(result);
}

void ivc::Neuron::sum() {
    float val_0 = m_inputWeights[0] * m_inputGates[0]->getValue();
    float val_1 = m_inputWeights[1] * m_inputGates[1]->getValue();
    float result = std::clamp(val_0 + val_1, -1.0f, 1.0f);
    m_output->setValue(result);
}

void ivc::Neuron::min() {
    float val_0 = m_inputWeights[0] * m_inputGates[0]->getValue();
    float val_1 = m_inputWeights[1] * m_inputGates[1]->getValue();

    float result = 0;
    if(val_0 < val_1){
        result = std::clamp(val_0, -1.0f, 1.0f);
        m_output->setValue(result);
    }else{
        result = std::clamp(val_1, -1.0f, 1.0f);
        m_output->setValue(result);
    }

}

void ivc::Neuron::max() {
    float val_0 = m_inputWeights[0] * m_inputGates[0]->getValue();
    float val_1 = m_inputWeights[1] * m_inputGates[1]->getValue();

    float result = 0;
    if(val_0 > val_1){
        result = std::clamp(val_0, -1.0f, 1.0f);
        m_output->setValue(result);
    }else{
        result = std::clamp(val_1, -1.0f, 1.0f);
        m_output->setValue(result);
    }
}

void ivc::Neuron::product() {
    float val_0 = m_inputWeights[0] * m_inputGates[0]->getValue();
    float val_1 = m_inputWeights[1] * m_inputGates[1]->getValue();
    float result = val_0 * val_1;
    result = std::clamp(result, -1.0f, 1.0f);

    m_output->setValue(result);
}

void ivc::Neuron::divide() {
    float val_0 = m_inputWeights[0] * m_inputGates[0]->getValue();
    float val_1 = m_inputWeights[1] * m_inputGates[1]->getValue();

    if(val_1 == 0)
        val_1 = 0.00001;

    float result = (val_0 / val_1);
    result = std::clamp(result, -1.0f, 1.0f);

    m_output->setValue(result);
}

void ivc::Neuron::sum_threshold() {
    float val_0 = m_inputWeights[0] * m_inputGates[0]->getValue();
    float val_1 = m_inputWeights[1] * m_inputGates[1]->getValue();

    float sum = val_0 + val_1;

    if(sum >= m_threshold)
        m_output->setValue(1.0f);
    else
        m_output->setValue(0.0f);

}

void ivc::Neuron::greater_than() {
    float val_0 = m_inputWeights[0] * m_inputGates[0]->getValue();
    float val_1 = m_inputWeights[1] * m_inputGates[1]->getValue();

    if(val_0 > val_1)
        m_output->setValue(1.0f);
    else
        m_output->setValue(0.0f);

}

void ivc::Neuron::if_then_else() {
    float val_0 = m_inputWeights[0] * m_inputGates[0]->getValue();
    float val_1 = m_inputWeights[1] * m_inputGates[1]->getValue();
    float val_2 = m_inputWeights[2] * m_inputGates[2]->getValue();

    if(val_0){
        m_output->setValue(val_1);
    }else{
        m_output->setValue(val_2);
    }
}

void ivc::Neuron::delay() {
    auto last = m_delayed_input;
    m_delayed_input = m_inputWeights[0] * m_inputGates[0]->getValue();
    m_output->setValue(last);
}

void ivc::Neuron::sigmoid() {
    float val_0 = m_inputWeights[0] * m_inputGates[0]->getValue();
    float val_1 = m_inputWeights[1] * m_inputGates[1]->getValue();
    float val_2 = m_inputWeights[2] * m_inputGates[2]->getValue();

    float sum = val_0 + val_1 + val_2 + m_genParam;

    m_output->setValue(1 / (1+std::exp(-sum)));
}

void ivc::Neuron::tanh() {
    float val_0 = m_inputWeights[0] * m_inputGates[0]->getValue();
    float val_1 = m_inputWeights[1] * m_inputGates[1]->getValue();
    float val_2 = m_inputWeights[2] * m_inputGates[2]->getValue();

    float sum = val_0 + val_1 + val_2 + m_genParam;

    m_output->setValue(std::tanh(sum));
}

std::vector<unsigned long> ivc::Neuron::getInputs() {
    return m_inputIDs;
}

void ivc::Neuron::setInputs(std::vector<unsigned long> newInputs) {
    if(newInputs.size() != m_numberInputs)
        return;

    m_inputIDs = newInputs;
}

ivc::NEURON_TYPE ivc::Neuron::getType() {
    return m_type;
}

