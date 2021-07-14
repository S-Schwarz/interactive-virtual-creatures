//
// Created by st on 7/12/21.
//

#include "JointEffector.h"

void ivc::JointEffector::step() {
    // TODO: apply force at joint
}

unsigned long ivc::JointEffector::getGateId() {
    return id_input;
}

int ivc::JointEffector::bindGate(ivc::Gate *gate) {
    if(gate == nullptr)
        return -1;
    input = gate;
    return 0;
}
