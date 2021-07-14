//
// Created by st on 7/12/21.
//

#include "JointSensor.h"

void ivc::JointSensor::step() {
    // TODO: get values from joint
}

void ivc::JointSensor::swap() {
    output_0.swap();
    output_1.swap();
    output_2.swap();
}

void ivc::JointSensor::setIDs(unsigned long id_0, unsigned long id_1, unsigned long id_2) {
    id_output_0 = id_0;
    id_output_1 = id_1;
    id_output_2 = id_2;
}
