//
// Created by st on 7/12/21.
//

#include "JointEffector.h"

void ivc::JointEffector::step() {
    float xVel = input_0->getValue() * weight_0;
    float yVel = input_1->getValue() * weight_1;
    float zVel = input_2->getValue() * weight_2;

    if(xVel > m_maxStrength){
        xVel = m_maxStrength;
    }else if(xVel < -m_maxStrength){
        xVel = -m_maxStrength;
    }

    if(yVel > m_maxStrength){
        yVel = m_maxStrength;
    }else if(yVel < -m_maxStrength){
        yVel = -m_maxStrength;
    }

    if(zVel > m_maxStrength){
        zVel = m_maxStrength;
    }else if(zVel < -m_maxStrength){
        zVel = -m_maxStrength;
    }

    m_joint->setDriveVelocity(PxVec3(0,0,0),PxVec3(xVel,yVel,zVel));

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

void ivc::JointEffector::setJoint(PxD6Joint *joint) {
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
    //mutate input weights
    weight_0 = Mutator::mutateFloat(gen, weight_0, INFINITY, -INFINITY);
    weight_1 = Mutator::mutateFloat(gen, weight_1, INFINITY, -INFINITY);
    weight_2 = Mutator::mutateFloat(gen, weight_2, INFINITY, -INFINITY);
}

void ivc::JointEffector::calculateMaxStrength(PxVec3 dimA, PxVec3 dimB) {

    float surfaceA = 2 * (dimA.x * dimA.y + dimA.x * dimA.z + dimA.y * dimA.z);
    float surfaceB = 2 * (dimB.x * dimB.y + dimB.x * dimB.z + dimB.y * dimB.z);

    if(surfaceA > surfaceB){
        m_maxStrength = surfaceB;
    }else{
        m_maxStrength = surfaceA;
    }



}
