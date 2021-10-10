//
// Created by st on 7/12/21.
//

#include "JointEffector.h"

void ivc::JointEffector::step() {
    float xVel = m_input_0->getValue() * m_weight_0 * MAX_JOINT_VELOCITY;
    float yVel = m_input_1->getValue() * m_weight_1 * MAX_JOINT_VELOCITY;
    float zVel = m_input_2->getValue() * m_weight_2 * MAX_JOINT_VELOCITY;

    addToHistory(PxVec3(xVel,yVel,zVel));

    auto average = getAverageValue();

    m_joint->setDriveVelocity(PxArticulationAxis::eTWIST, average.x);
    m_joint->setDriveVelocity(PxArticulationAxis::eSWING1, average.y);
    m_joint->setDriveVelocity(PxArticulationAxis::eSWING2, average.z);
}

std::vector<unsigned long> ivc::JointEffector::getGateIDs() {
    return {m_id_input_0, m_id_input_1, m_id_input_2};
}

int ivc::JointEffector::bindGates(std::vector<std::shared_ptr<Gate>> gates) {

    if(gates.size() != 3)
        return -1;

    m_input_0 = gates[0];
    m_input_1 = gates[1];
    m_input_2 = gates[2];

    return 0;
}

void ivc::JointEffector::setJoint(PxArticulationJointReducedCoordinate* joint) {
    m_joint = joint;
}

void ivc::JointEffector::chooseRandomInputs(std::vector<unsigned long> possibleInputs) {

    if(possibleInputs.empty())
        return;

    static auto rng = std::default_random_engine(std::chrono::system_clock::now().time_since_epoch().count());

    std::shuffle(std::begin(possibleInputs), std::end(possibleInputs), rng);
    m_id_input_0 = possibleInputs[0];

    std::shuffle(std::begin(possibleInputs), std::end(possibleInputs), rng);
    m_id_input_1 = possibleInputs[0];

    std::shuffle(std::begin(possibleInputs), std::end(possibleInputs), rng);
    m_id_input_2 = possibleInputs[0];
}

void ivc::JointEffector::mutate(std::shared_ptr<std::mt19937> gen, std::shared_ptr<EvoConfig> config) {

    std::uniform_real_distribution<> dis(0, 1);

    //mutateBodyAndNeurons input weights
    if(dis(*gen) <= config->m_mutChance)
        m_weight_0 = Mutator::mutateFloat(gen, m_weight_0, 1.0f, -1.0f);
    if(dis(*gen) <= config->m_mutChance)
        m_weight_1 = Mutator::mutateFloat(gen, m_weight_1, 1.0f, -1.0f);
    if(dis(*gen) <= config->m_mutChance)
        m_weight_2 = Mutator::mutateFloat(gen, m_weight_2, 1.0f, -1.0f);
}

void ivc::JointEffector::mutateConnections(std::shared_ptr<std::mt19937> gen, std::vector<unsigned long> possibleInputs, std::shared_ptr<EvoConfig> config) {
    std::uniform_real_distribution<> dis(0, 1);

    //mutateBodyAndNeurons input connections
    if(possibleInputs.empty())
        return;

    static auto rng = std::default_random_engine(std::chrono::system_clock::now().time_since_epoch().count());

    if(std::find(possibleInputs.begin(), possibleInputs.end(), m_id_input_0) == possibleInputs.end() || dis(*gen) <= config->m_mutChance){
        std::shuffle(std::begin(possibleInputs), std::end(possibleInputs), rng);
        m_id_input_0 = possibleInputs[0];
    }

    if(std::find(possibleInputs.begin(), possibleInputs.end(), m_id_input_1) == possibleInputs.end() || dis(*gen) <= config->m_mutChance){
        std::shuffle(std::begin(possibleInputs), std::end(possibleInputs), rng);
        m_id_input_1 = possibleInputs[0];
    }

    if(std::find(possibleInputs.begin(), possibleInputs.end(), m_id_input_2) == possibleInputs.end() || dis(*gen) <= config->m_mutChance){
        std::shuffle(std::begin(possibleInputs), std::end(possibleInputs), rng);
        m_id_input_2 = possibleInputs[0];
    }
}

void ivc::JointEffector::addToHistory(PxVec3 newValue) {

    if(m_lastValues.size() < NUMBER_SAVED_EFFECTOR_VALUES){
        m_lastValues.push_back(newValue);
    }else{

        m_lastValues[m_lastValuesIndex] = newValue;

        if(m_lastValuesIndex == NUMBER_SAVED_EFFECTOR_VALUES-1){
            m_lastValuesIndex = 0;
        }else{
            ++m_lastValuesIndex;
        }
    }

}

PxVec3 ivc::JointEffector::getAverageValue() {

    PxVec3 averageVec(0,0,0);

    for(auto vec : m_lastValues){
        averageVec += vec;
    }

    averageVec /= m_lastValues.size();

    return averageVec;
}

void ivc::JointEffector::setInputs(std::vector<unsigned long> newInputs) {

    if(newInputs.size() != 3)
        return;

    m_id_input_0 = newInputs[0];
    m_id_input_1 = newInputs[1];
    m_id_input_2 = newInputs[2];

}

std::vector<unsigned long> ivc::JointEffector::getInputs() {
    return {m_id_input_0, m_id_input_1, m_id_input_2};
}
