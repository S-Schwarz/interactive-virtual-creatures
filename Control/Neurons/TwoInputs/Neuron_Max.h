//
// Created by st on 7/12/21.
//

#ifndef INTERACTIVE_VIRTUAL_CREATURES_NEURON_MAX_H
#define INTERACTIVE_VIRTUAL_CREATURES_NEURON_MAX_H


#include "../Neuron.h"
#include "../../Gate.h"
#include "Neuron_TwoInputs.h"

namespace ivc{
    class Neuron_Max : public Neuron_TwoInputs{
        private:
            void max();
        public:
            Neuron* copy() override;
            void step() override;
    };
}


#endif //INTERACTIVE_VIRTUAL_CREATURES_NEURON_MAX_H
