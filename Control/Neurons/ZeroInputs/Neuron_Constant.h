//
// Created by st on 7/15/21.
//

#ifndef INTERACTIVE_VIRTUAL_CREATURES_NEURON_CONSTANT_H
#define INTERACTIVE_VIRTUAL_CREATURES_NEURON_CONSTANT_H

#include "../Neuron.h"
namespace ivc{
    class Neuron_Constant : public Neuron {
    private:
        float m_constant = 1.0f;
    public:
        Neuron* copy() override;
        void step() override;
        void randomize(std::mt19937*) override;
    };
}



#endif //INTERACTIVE_VIRTUAL_CREATURES_NEURON_CONSTANT_H
