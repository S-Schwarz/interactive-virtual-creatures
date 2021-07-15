//
// Created by st on 7/12/21.
//

#include "JointEffector.h"

void ivc::JointEffector::step() {
    m_joint->setDriveVelocity(PxVec3(0,0,0),PxVec3(input_0->getValue() * weight_0,input_1->getValue() * weight_1,input_2->getValue() * weight_2));
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
