//
// Created by st on 7/12/21.
//

#include "JointSensor.h"

ivc::JointSensor::JointSensor(PxD6Joint *joint) {
    input_joint = joint;
}

void ivc::JointSensor::step() {
    // TODO: get values from joint
}

void ivc::JointSensor::swap() {
    output_0.swap();
    output_1.swap();
    output_2.swap();
}