//
// Created by st on 7/21/21.
//

#include "ContactSensor.h"

void ivc::ContactSensor::setOutputGates(std::vector<Gate *> gates) {
    if(gates.size() != 6)
        return;
    m_outputGates = gates;

}

void ivc::ContactSensor::setIDs(std::vector<unsigned long> ids) {
    if(ids.size() != 6)
        return;
    m_outputIDs = ids;
}

std::vector<unsigned long> ivc::ContactSensor::getOutputIDs() {
    return m_outputIDs;
}

void ivc::ContactSensor::swap() {
    for(auto gate : m_outputGates){
        gate->swap();
    }
}

void ivc::ContactSensor::step() {
    for(int i = 0; i < 6; ++i){
        if(m_hasContact[i])
            m_outputGates[i]->setValue(1.0f);
        else
            m_outputGates[i]->setValue(-1.0f);
    }
}

void ivc::ContactSensor::initContactVec() {
    for(int i = 0; i < 6; ++i){
        m_hasContact.push_back(false);
    }
}

bool ivc::ContactSensor::contains(std::string id_string) {
    for(auto id : m_outputIDs){
        if(std::to_string(id) == id_string)
            return true;
    }
    return false;
}

void ivc::ContactSensor::setState(std::string id_string, bool state) {
    for(int i = 0; i < 6; ++i){
        if(std::to_string(m_outputIDs[i]) == id_string){
            m_hasContact[i] = state;
            return;
        }
    }
}
