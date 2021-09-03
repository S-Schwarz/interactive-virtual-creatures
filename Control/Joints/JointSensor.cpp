//
// Created by st on 7/12/21.
//

#include "JointSensor.h"

void ivc::JointSensor::step() {
    auto index = m_link->getLinkIndex();
    m_output_0->setValue(m_cache->jointPosition[index] * m_weight_0);
    m_output_1->setValue(m_cache->jointPosition[index + 1] * m_weight_1);
    m_output_2->setValue(m_cache->jointPosition[index + 2] * m_weight_2);
}

void ivc::JointSensor::swap() {
    m_output_0->swap();
    m_output_1->swap();
    m_output_2->swap();
}

void ivc::JointSensor::setIDs(unsigned long id_0, unsigned long id_1, unsigned long id_2) {
    m_id_output_0 = id_0;
    m_id_output_1 = id_1;
    m_id_output_2 = id_2;
}

std::vector<unsigned long> ivc::JointSensor::getOutputIDs() {
    return {m_id_output_0, m_id_output_1, m_id_output_2};
}

void ivc::JointSensor::randomize(std::mt19937* gen) {
    std::normal_distribution<> outputDis(MEAN_SENSOR_WEIGHT, MEAN_SENSOR_WEIGHT * STANDARD_DEVIATION_FACTOR);
    m_weight_0 = outputDis(*gen);
    m_weight_1 = outputDis(*gen);
    m_weight_2 = outputDis(*gen);
}

void ivc::JointSensor::setOutputGates(std::vector<Gate *> gates) {

    if(gates.size() != 3)
        return;

    m_output_0 = gates[0];
    m_output_1 = gates[1];
    m_output_2 = gates[2];

}

void ivc::JointSensor::mutate(std::mt19937 *gen) {

    std::uniform_real_distribution<> dis(0, 1);

    //mutateBodyAndNeurons output weights
    if(dis(*gen) <= MUTATE_OUTPUT_WEIGHT_CHANCE)
        m_weight_0 = Mutator::mutateFloat(gen, m_weight_0, INFINITY, -INFINITY);
    if(dis(*gen) <= MUTATE_OUTPUT_WEIGHT_CHANCE)
        m_weight_1 = Mutator::mutateFloat(gen, m_weight_1, INFINITY, -INFINITY);
    if(dis(*gen) <= MUTATE_OUTPUT_WEIGHT_CHANCE)
        m_weight_2 = Mutator::mutateFloat(gen, m_weight_2, INFINITY, -INFINITY);
}

void ivc::JointSensor::setCache(PxArticulationCache* cache) {
    m_cache = cache;
}

void ivc::JointSensor::setLink(PxArticulationLink* link) {
    m_link = link;
}

void ivc::JointSensor::setIDs(std::vector<unsigned long> idVec) {
    if(idVec.size() != 3)
        return;

    m_id_output_0 = idVec[0];
    m_id_output_1 = idVec[1];
    m_id_output_2 = idVec[2];
}

std::vector<ivc::Gate *> ivc::JointSensor::getOutputGates() {
    return {m_output_0, m_output_1, m_output_2};
}
