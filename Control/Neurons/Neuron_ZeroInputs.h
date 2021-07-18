//
// Created by st on 7/18/21.
//

#ifndef INTERACTIVE_VIRTUAL_CREATURES_NEURON_ZEROINPUTS_H
#define INTERACTIVE_VIRTUAL_CREATURES_NEURON_ZEROINPUTS_H

#include "Neuron.h"

namespace ivc{
    class Neuron_ZeroInputs : public Neuron{
    private:
        NEURON_TYPE m_type;
        float m_constant = 1.0f;
    public:
        Neuron_ZeroInputs(NEURON_TYPE);
        Neuron* copy() override;
        void step() override;
        void randomize(std::mt19937*) override;
        void mutate(std::mt19937*) override;
        void mutateConnections(std::mt19937*,std::vector<unsigned long>) override;
    };
}



#endif //INTERACTIVE_VIRTUAL_CREATURES_NEURON_ZEROINPUTS_H
