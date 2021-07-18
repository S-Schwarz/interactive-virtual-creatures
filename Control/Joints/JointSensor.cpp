//
// Created by st on 7/12/21.
//

#include "JointSensor.h"

void ivc::JointSensor::step() {
    output_0->setValue(m_joint->getTwistAngle() * weight_0);
    output_1->setValue(m_joint->getSwingYAngle() * weight_1);
    output_2->setValue(m_joint->getSwingZAngle() * weight_2);
}

void ivc::JointSensor::swap() {
    output_0->swap();
    output_1->swap();
    output_2->swap();
}

void ivc::JointSensor::setIDs(unsigned long id_0, unsigned long id_1, unsigned long id_2) {
    id_output_0 = id_0;
    id_output_1 = id_1;
    id_output_2 = id_2;
}

std::vector<unsigned long> ivc::JointSensor::getOutputIDs() {
    return {id_output_0,id_output_1,id_output_2};
}

void ivc::JointSensor::setJoint(PxD6Joint *joint) {
    m_joint = joint;
}

void ivc::JointSensor::randomize(std::mt19937* gen) {
    std::normal_distribution<> outputDis(MEAN_SENSOR_WEIGHT, MEAN_SENSOR_WEIGHT * STANDARD_DEVIATION_FACTOR);
    weight_0 = outputDis(*gen);
    weight_1 = outputDis(*gen);
    weight_2 = outputDis(*gen);
}

void ivc::JointSensor::setOutputGates(std::vector<Gate *> gates) {

    if(gates.size() != 3)
        return;

    output_0 = gates[0];
    output_1 = gates[1];
    output_2 = gates[2];

}

void ivc::JointSensor::mutate(std::mt19937 *gen) {

    std::uniform_real_distribution<> dis(0, 1);

    //mutateBodyAndNeurons output weights
    if(dis(*gen) <= MUTATE_OUTPUT_WEIGHT_CHANCE)
        weight_0 = Mutator::mutateFloat(gen, weight_0, INFINITY, -INFINITY);
    if(dis(*gen) <= MUTATE_OUTPUT_WEIGHT_CHANCE)
        weight_1 = Mutator::mutateFloat(gen, weight_1, INFINITY, -INFINITY);
    if(dis(*gen) <= MUTATE_OUTPUT_WEIGHT_CHANCE)
        weight_2 = Mutator::mutateFloat(gen, weight_2, INFINITY, -INFINITY);
}
