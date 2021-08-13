//
// Created by st on 7/12/21.
//

#include "JointEffector.h"

void ivc::JointEffector::step() {
    float xVel = input_0->getValue() * weight_0;
    float yVel = input_1->getValue() * weight_1;
    float zVel = input_2->getValue() * weight_2;

    if(xVel > MAX_JOINT_VELOCITY)
        xVel = MAX_JOINT_VELOCITY;
    else if(xVel < -MAX_JOINT_VELOCITY)
        xVel = -MAX_JOINT_VELOCITY;

    if(yVel > MAX_JOINT_VELOCITY)
        yVel = MAX_JOINT_VELOCITY;
    else if(yVel < -MAX_JOINT_VELOCITY)
        yVel = -MAX_JOINT_VELOCITY;

    if(zVel > MAX_JOINT_VELOCITY)
        zVel = MAX_JOINT_VELOCITY;
    else if(zVel < -MAX_JOINT_VELOCITY)
        zVel = -MAX_JOINT_VELOCITY;

    m_joint->setDriveVelocity(PxArticulationAxis::eTWIST, xVel);
    m_joint->setDriveVelocity(PxArticulationAxis::eSWING1, yVel);
    m_joint->setDriveVelocity(PxArticulationAxis::eSWING2, zVel);
}

std::vector<unsigned long> ivc::JointEffector::getGateIDs() {
    return {id_input_0,id_input_1,id_input_2};
}

int ivc::JointEffector::bindGates(std::vector<Gate *> gates) {

    if(gates.size() != 3)
        return -1;

    input_0 = gates[0];
    input_1 = gates[1];
    input_2 = gates[2];

    return 0;
}

void ivc::JointEffector::setJoint(PxArticulationJointReducedCoordinate *joint) {
    m_joint = joint;
}

void ivc::JointEffector::randomize(std::mt19937* gen) {
    std::normal_distribution<> outputDis(MEAN_EFFECTOR_WEIGHT, MEAN_EFFECTOR_WEIGHT * STANDARD_DEVIATION_FACTOR);
    weight_0 = outputDis(*gen);
    weight_1 = outputDis(*gen);
    weight_2 = outputDis(*gen);
}

void ivc::JointEffector::chooseRandomInputs(std::vector<unsigned long> possibleInputs) {

    if(possibleInputs.empty())
        return;

    static auto rng = std::default_random_engine(std::chrono::system_clock::now().time_since_epoch().count());

    std::shuffle(std::begin(possibleInputs), std::end(possibleInputs), rng);
    id_input_0 = possibleInputs[0];

    std::shuffle(std::begin(possibleInputs), std::end(possibleInputs), rng);
    id_input_1 = possibleInputs[0];

    std::shuffle(std::begin(possibleInputs), std::end(possibleInputs), rng);
    id_input_2 = possibleInputs[0];
}

void ivc::JointEffector::mutate(std::mt19937 *gen) {

    std::uniform_real_distribution<> dis(0, 1);

    //mutateBodyAndNeurons input weights
    if(dis(*gen) <= MUTATE_INPUT_WEIGHT_CHANCE)
        weight_0 = Mutator::mutateFloat(gen, weight_0, INFINITY, -INFINITY);
    if(dis(*gen) <= MUTATE_INPUT_WEIGHT_CHANCE)
        weight_1 = Mutator::mutateFloat(gen, weight_1, INFINITY, -INFINITY);
    if(dis(*gen) <= MUTATE_INPUT_WEIGHT_CHANCE)
        weight_2 = Mutator::mutateFloat(gen, weight_2, INFINITY, -INFINITY);
}

void ivc::JointEffector::mutateConnections(std::mt19937 *gen, std::vector<unsigned long> possibleInputs) {
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

    if(std::find(possibleInputs.begin(), possibleInputs.end(), id_input_2) == possibleInputs.end() || dis(*gen) <= MUTATE_CONNECTION_CHANCE){
        std::shuffle(std::begin(possibleInputs), std::end(possibleInputs), rng);
        id_input_2 = possibleInputs[0];
    }
}
