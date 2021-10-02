//
// Created by st on 7/12/21.
//

#include "JointSensor.h"

void ivc::JointSensor::step() {
    auto index = m_link->getLinkIndex();

    float jointPos = m_cache->jointPosition[index];

    jointPos = Mutator::normalize(jointPos, -PxTwoPi, PxTwoPi);

    m_output->setValue(jointPos);
}

void ivc::JointSensor::swap() {
    m_output->swap();
}

void ivc::JointSensor::setID(unsigned long id) {
    m_id_output = id;
}

unsigned long ivc::JointSensor::getOutputID() {
    return m_id_output;
}

void ivc::JointSensor::setOutputGate(Gate * gate) {

    if(gate == nullptr)
        return;

    m_output = gate;

}

void ivc::JointSensor::setCache(PxArticulationCache* cache) {
    m_cache = cache;
}

void ivc::JointSensor::setLink(PxArticulationLink* link) {
    m_link = link;
}

ivc::Gate* ivc::JointSensor::getOutputGate() {
    return m_output;
}
