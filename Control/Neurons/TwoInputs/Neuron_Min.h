//
// Created by st on 7/12/21.
//

#ifndef INTERACTIVE_VIRTUAL_CREATURES_NEURON_MIN_H
#define INTERACTIVE_VIRTUAL_CREATURES_NEURON_MIN_H

#include "../Neuron.h"
#include "../../Gate.h"
#include "Neuron_TwoInputs.h"

namespace ivc{
    class Neuron_Min : public Neuron_TwoInputs{
        private:
            void min();
        public:
            Neuron* copy() override;
            void step() override;
    };
}



#endif //INTERACTIVE_VIRTUAL_CREATURES_NEURON_MIN_H
