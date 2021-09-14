//
// Created by st on 7/12/21.
//

#include <cstdio>
#include "Gate.h"

void ivc::Gate::setValue(float val) {
    if(val > 1 || val < -1)
        printf("FUCK IN GATE\n");
    m_newValue = val;
}

float ivc::Gate::getValue() {
    return m_oldValue;
}

void ivc::Gate::swap() {
    m_oldValue = m_newValue;
}