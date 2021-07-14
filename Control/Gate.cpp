//
// Created by st on 7/12/21.
//

#include "Gate.h"

void ivc::Gate::setValue(float val) {
    m_newValue = val;
}

float ivc::Gate::getValue() {
    return m_oldValue;
}

void ivc::Gate::swap() {
    m_oldValue = m_newValue;
}