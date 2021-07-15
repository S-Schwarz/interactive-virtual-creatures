//
// Created by st on 7/12/21.
//

#include "Neuron_Max.h"

void ivc::Neuron_Max::step() {
    max();
}

void ivc::Neuron_Max::max() {

    float val_0 = weight_0 * input_0->getValue();
    float val_1 = weight_1 * input_1->getValue();

    if(val_0 > val_1)
        output.setValue(m_outputWeight * val_0);
    else
        output.setValue(m_outputWeight * val_1);

}

ivc::Neuron *ivc::Neuron_Max::copy() {
    return new Neuron_Max(*this);
}
