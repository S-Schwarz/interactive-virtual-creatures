//
// Created by st on 7/12/21.
//

#include "JointEffector.h"

void ivc::JointEffector::step() {
    float xVel = input_0->getValue() * weight_0;
    float yVel = input_1->getValue() * weight_1;
    float zVel = input_2->getValue() * weight_2;

    float maxStrength = 10000.0f;

    if(xVel > maxStrength){
        xVel = maxStrength;
    }else if(xVel < -maxStrength){
        xVel = -maxStrength;
    }

    if(yVel > maxStrength){
        yVel = maxStrength;
    }else if(yVel < -maxStrength){
        yVel = -maxStrength;
    }
    
    if(zVel > maxStrength){
        zVel = maxStrength;
    }else if(zVel < -maxStrength){
        zVel = -maxStrength;
    }

    printf("X: %f\n", xVel);
    printf("Y: %f\n", yVel);
    printf("Z: %f\n", zVel);

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
